namespace MMO_ApiServer_ASP.Model
{
    public class AccountDTO
    {
        public int AccountCode { get; set; }
        public int Cash { get; set; }
        public int WeaponOne { get; set; }
        public int WeaponTwo { get; set; }
        public int WeaponThr { get; set; }
        public int CurPlayerType { get; set; }
        public int CurWeaponType { get; set; }
    }

    public class AccountResponse
    {
        public int Ret { get; set; }
        public AccountDTO Account { get; set; }
    }

    public class PlayerRespons
    {
        public int Ret { get; set; }
        public PlayerDTO Player { get; set; }
    }

    public class PlayersRespons
    {
        public int Ret { get; set; }
        public List<PlayerDTO> Players { get; set; }
    }

    public class PlayerDTO
    {
        public int PlayerCode { get; set; }
        public string Name { get; set; }
        public int JobCode { get; set; }
        public int MapCode { get; set; }
        public int Lv { get; set; }
        public int Exp { get; set; }
        public int Gold { get; set; }
    }
}
