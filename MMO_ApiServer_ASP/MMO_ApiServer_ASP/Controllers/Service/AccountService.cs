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

        public void Login(string id, string pwd, JsonObject obj)
        {
            Console.WriteLine($"LoginService {id} - {pwd}");

            IQueryable<Account> results = Context.Accounts.Where(a => a.id == id);

            if (results.Count() == 1)
            {
                Account account = results.FirstOrDefault();
                if (account.pwd == pwd)
                {
                    obj["ret"] = "1";
                    obj["cash"] = account.cash;
                    obj["weaponOne"] = account.weaponOne;
                    obj["weaponTwo"] = account.weaponTwo;
                    obj["weaponThr"] = account.weaponThr;
                    obj["curPlayerType"] = account.curPlayerType;
                    obj["curWeaponType"] = account.curWeaponType;   
                    obj["accountCode"] = account.accountCode;
                }
            }
            else
            {
                Account account = new Account { id = id, pwd = pwd, cash = 10000, weaponOne = 0, weaponTwo = 0, weaponThr = 0, curPlayerType = 0, curWeaponType = 0 };
                Context.Add(account);
                int affectRows = Context.SaveChanges();
                if (affectRows > 0)
                {
                    obj["ret"] = "2";
                }
            }
        }

        public void GetAccount(int accountCode, JsonObject obj)
        {
            IQueryable<Account> results = Context.Accounts.Where(a => a.accountCode == accountCode);
            if (results.Count() == 1)
            {
                Account account = results.FirstOrDefault();
                obj["ret"] = "1";
                obj["cash"] = account.cash;
                obj["weaponOne"] = account.weaponOne;
                obj["weaponTwo"] = account.weaponTwo;
                obj["weaponThr"] = account.weaponThr;
                obj["curPlayerType"] = account.curPlayerType;
                obj["curWeaponType"] = account.curWeaponType;
                obj["accountCode"] = account.accountCode;
            }
        }

        public void GetPlayers(int accountCode, JsonObject obj)
        {
            IQueryable<Player> results = Context.Players.Where(a => a.accountCode == accountCode);

            if (results.Count() > 0)
            {
                JsonArray players = new JsonArray();
                foreach (Player player in results)
                {
                    JsonObject playerObj = new JsonObject();
                    playerObj["playerCode"] = player.playerCode;
                    playerObj["name"] = player.name;
                    playerObj["jobCode"] = player.jobCode;
                    playerObj["mapCode"] = player.mapCode;
                    playerObj["lv"] = player.lv;
                    playerObj["exp"] = player.exp;
                    playerObj["gold"] = player.gold;
                    players.Add(playerObj);
                }
                obj["characters"] = players;
            }
        }

        public void GetPlayer(int accountCode, int type, JsonObject obj)
        {
            IQueryable<Player> results = Context.Players.Where(a => a.accountCode == accountCode);

            if (results.Count() == 1)
            {
                JsonArray players = new JsonArray();
                Player player = results.FirstOrDefault();
                if (player != null)
                {
                    JsonObject playerObj = new JsonObject();
                    playerObj["playerCode"] = player.playerCode;
                    playerObj["name"] = player.name;
                    playerObj["jobCode"] = player.jobCode;
                    playerObj["mapCode"] = player.mapCode;
                    playerObj["lv"] = player.lv;
                    playerObj["exp"] = player.exp;
                    playerObj["gold"] = player.gold;
                    players.Add(playerObj);
                    obj["characters"] = players;
                }
            }
        }

        public void UpdateAccount(int accountCode, int characterType, int weaponType, JsonObject obj)
        {
            IQueryable<Account> results = Context.Accounts.Where(a => a.accountCode == accountCode);
            if (results.Count() == 1)
            {
                Account account = results.FirstOrDefault();
                account.curPlayerType = characterType;
                account.curWeaponType = weaponType;
                Context.Update(account);
                Context.SaveChanges();

                obj["ret"] = "1";
                obj["cash"] = account.cash;
                obj["weaponOne"] = account.weaponOne;
                obj["weaponTwo"] = account.weaponTwo;
                obj["weaponThr"] = account.weaponThr;
                obj["curPlayerType"] = account.curPlayerType;
                obj["curWeaponType"] = account.curWeaponType;
                obj["accountCode"] = account.accountCode;
            }
        }

        public void BuyCharacter(int accountCode, int useCash, int characterType, string characterName, JsonObject obj)
        {
            IQueryable<Account> results = Context.Accounts.Where(a => a.accountCode == accountCode);
            if (results.Count() == 1)
            {
                Account account = results.FirstOrDefault();
                if (account.cash >= useCash)
                {
                    IQueryable<Player> subResults = Context.Players.Where(a => a.accountCode == accountCode && a.jobCode == characterType);
                    if (subResults.Count() > 0)
                    {
                        return;
                    }

                    Player player = new Player { accountCode = accountCode, name = characterName, jobCode = characterType, mapCode = 0, lv = 1, exp = 0, gold = 0 };
                    account.cash -= useCash;
                    Context.Add(player);
                    Context.Update(account);
                    int affectRows = Context.SaveChanges();
                    if (affectRows >= 2)
                    {
                        obj["ret"] = "1";
                    }   
                }
            }
        }

        public void BuyWeapon(int accountCode, int useCash, int weaponType, JsonObject obj)
        {
            IQueryable<Account> results = Context.Accounts.Where(a => a.accountCode == accountCode);
            if (results.Count() == 1)
            {
                Account account = results.FirstOrDefault();
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
                            obj["ret"] = "1";
                        }
                    }
                }
            }
        }
    }
}
