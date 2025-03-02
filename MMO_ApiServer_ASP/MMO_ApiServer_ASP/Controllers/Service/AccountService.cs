using Microsoft.AspNetCore.Http;
using Microsoft.EntityFrameworkCore;
using MMO_ApiServer_ASP.Context;
using MMO_ApiServer_ASP.Model;
using System.Collections.Generic;
using System.Security.Principal;
using System.Text.Json.Nodes;

namespace MMO_ApiServer_ASP.Controllers.Service
{
    public class AccountService
    {
        ApplicationDbContext Context { get; set; }

        public AccountService(ApplicationDbContext context)
        {
            Context = context;
        }


        public AccountResponse Login(string id, string pwd)
        {
            AccountResponse response = new AccountResponse { Ret = 0 };

            IQueryable<Data> results = Context.Accounts.Where(a => a.id == id);
            if (results.Count() == 1)
            {
                Data account = results.FirstOrDefault();
                if (account.pwd == pwd)
                {
                    response.Ret = 1;
                    response.Account = new AccountDTO
                    {
                        AccountCode = account.accountCode,
                        Cash = account.cash,
                        WeaponOne = account.weaponOne,
                        WeaponTwo = account.weaponTwo,
                        WeaponThr = account.weaponThr,
                        CurPlayerType = account.curPlayerType,
                        CurWeaponType = account.curWeaponType
                    };
                }
            }
            else
            {
                Data account = new Data { id = id, pwd = pwd, cash = 10000, weaponOne = 0, weaponTwo = 0, weaponThr = 0, curPlayerType = 0, curWeaponType = 0 };
                Context.Add(account);
                int affectRows = Context.SaveChanges();
                if (affectRows > 0)
                {
                    response.Ret = 2;
                }
            }

            return response;
        }

        public AccountResponse GetAccount(int accountCode)
        {
            AccountResponse response = new AccountResponse { Ret = 0 };
            IQueryable<Data> results = Context.Accounts.Where(a => a.accountCode == accountCode);
            if (results.Count() == 1)
            {
                Data account = results.FirstOrDefault();
                response.Ret = 1;
                response.Account = new AccountDTO
                {
                    AccountCode = account.accountCode,
                    Cash = account.cash,
                    WeaponOne = account.weaponOne,
                    WeaponTwo = account.weaponTwo,
                    WeaponThr = account.weaponThr,
                    CurPlayerType = account.curPlayerType,
                    CurWeaponType = account.curWeaponType
                };
            }
            return response;
        }

        public PlayersRespons GetPlayers(int accountCode)
        {
            PlayersRespons response = new PlayersRespons { Ret = 0 };
            IQueryable<Player> results = Context.Players.Where(a => a.accountCode == accountCode);

            if (results.Count() > 0)
            {
                response.Ret = 1;
                response.Players = new List<PlayerDTO>();
                foreach (Player player in results)
                {
                    response.Players.Add(new PlayerDTO
                    {
                        PlayerCode = player.playerCode,
                        Name = player.name,
                        JobCode = player.jobCode,
                        MapCode = player.mapCode,
                        Lv = player.lv,
                        Exp = player.exp,
                        Gold = player.gold
                    });
                }
            }
            return response;
        }

        public PlayerRespons GetPlayer(int accountCode, int type)
        {
            PlayerRespons response = new PlayerRespons { Ret = 0 };
            IQueryable<Player> results = Context.Players.Where(a => a.accountCode == accountCode && a.jobCode == type);

            if (results.Count() == 1)
            {
                Player player = results.FirstOrDefault();
                if (player != null)
                {
                    response.Ret = 1;
                    response.Player = new PlayerDTO
                    {
                        PlayerCode = player.playerCode,
                        Name = player.name,
                        JobCode = player.jobCode,
                        MapCode = player.mapCode,
                        Lv = player.lv,
                        Exp = player.exp,
                        Gold = player.gold
                    };
                }
            }
            return response;
        }

        public AccountResponse UpdateAccount(int accountCode, int characterType, int weaponType)
        {
            AccountResponse response = new AccountResponse { Ret = 0 };
            IQueryable<Data> results = Context.Accounts.Where(a => a.accountCode == accountCode);
            if (results.Count() == 1)
            {
                Data account = results.FirstOrDefault();
                if (characterType > 0)
                {
                    IQueryable<Player> subResults = Context.Players.Where(a => a.accountCode == accountCode && a.jobCode == characterType);
                    if (subResults.Count() == 0)
                    {
                        characterType = account.curPlayerType;
                    }
                }
                else
                {
                    characterType = account.curPlayerType;
                }

                if (weaponType > 0)
                {
                    if (weaponType == 1 && account.weaponOne == 0)
                        weaponType = account.curWeaponType;
                    else if (weaponType == 2 && account.weaponTwo == 0)
                        weaponType = account.curWeaponType;
                    else if (weaponType == 3 && account.weaponThr == 0)
                        weaponType = account.curWeaponType;
                }
                else
                {
                    weaponType = account.curWeaponType;
                }

                account.curPlayerType = characterType;
                account.curWeaponType = weaponType;
                Context.Update(account);
                Context.SaveChanges();

                response.Ret = 1;
                response.Account = new AccountDTO
                {
                    AccountCode = account.accountCode,
                    Cash = account.cash,
                    WeaponOne = account.weaponOne,
                    WeaponTwo = account.weaponTwo,
                    WeaponThr = account.weaponThr,
                    CurPlayerType = account.curPlayerType,
                    CurWeaponType = account.curWeaponType
                };
            }
            return response;
        }

        public AccountResponse BuyCharacter(int accountCode, int useCash, int characterType, string characterName)
        {
            AccountResponse response = new AccountResponse { Ret = 0 };
            using (var transaction = Context.Database.BeginTransaction()) // 트랜잭션 시작
            {
                try
                {
                    IQueryable<Data> results = Context.Accounts.Where(a => a.accountCode == accountCode);
                    if (results.Count() == 1)
                    {
                        Data account = results.FirstOrDefault();
                        if (account.cash >= useCash)
                        {
                            IQueryable<Player> subResults = Context.Players.Where(a => a.accountCode == accountCode && a.jobCode == characterType);
                            if (subResults.Count() == 0)
                            {
                                Player player = new Player { accountCode = accountCode, name = characterName, jobCode = characterType, mapCode = 0, lv = 1, exp = 0, gold = 0 };
                                account.cash -= useCash;
                                Context.Add(player);
                                Context.Update(account);
                                int affectRows = Context.SaveChanges();
                                if (affectRows >= 2)
                                {
                                    transaction.Commit(); // 트랜잭션 커밋 2개다 성공해야됨

                                    response.Ret = 1;
                                    response.Account = new AccountDTO
                                    {
                                        AccountCode = account.accountCode,
                                        Cash = account.cash,
                                        WeaponOne = account.weaponOne,
                                        WeaponTwo = account.weaponTwo,
                                        WeaponThr = account.weaponThr,
                                        CurPlayerType = account.curPlayerType,
                                        CurWeaponType = account.curWeaponType
                                    };
                                }
                            }
                            else
                            {
                                response.Ret = -1; // 이미 해당 타입의 캐릭터가 존재하는 경우
                                transaction.Rollback();
                            }
                        }
                    }
                }
                catch (System.Exception)
                {
                    transaction.Rollback();
                }
            }
            return response;
        }

        public AccountResponse BuyWeapon(int accountCode, int useCash, int weaponType)
        {
            AccountResponse response = new AccountResponse { Ret = 0 };
            IQueryable<Data> results = Context.Accounts.Where(a => a.accountCode == accountCode);
            if (results.Count() == 1)
            {
                Data account = results.FirstOrDefault();
                if (account.cash >= useCash)
                {
                    bool flag = true;
                    switch (weaponType)
                    {
                        case 1:
                            if (account.weaponOne == 1)
                            {
                                flag = false;
                            }
                            account.weaponOne = 1;
                            break;
                        case 2:
                            if (account.weaponTwo == 1)
                            {
                                flag = false;
                            }
                            account.weaponTwo = 1;
                            break;
                        case 3:
                            if (account.weaponThr == 1)
                            {
                                flag = false;
                            }
                            account.weaponThr = 1;
                            break;
                    }
                    if (flag)
                    {
                        account.cash -= useCash;
                        Context.Update(account);
                        int affectRows = Context.SaveChanges();
                        if (affectRows > 0)
                        {
                            response.Ret = 1;
                            response.Account = new AccountDTO
                            {
                                AccountCode = account.accountCode,
                                Cash = account.cash,
                                WeaponOne = account.weaponOne,
                                WeaponTwo = account.weaponTwo,
                                WeaponThr = account.weaponThr,
                                CurPlayerType = account.curPlayerType,
                                CurWeaponType = account.curWeaponType
                            };
                        }
                    }
                    else
                    {
                        response.Ret = -1; // 이미 해당 타입의 무기가 존재하는 경우
                    }
                }
            }
            return response;
        }
    }
}
