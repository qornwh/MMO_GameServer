namespace MMO_ApiServer_ASP.Model
{
    public class LoginRequest
    {
        public string? Id { get; set; }
        public string? Pwd { get; set; }
    }
    public class PlayerRequest
    {
        public int AccountCode { get; set; }
        public int Type { get; set; }
    }
    public class UpdateAccountRequest
    {
        public int AccountCode { get; set; }
        public int CharacterType { get; set; }
        public int WeaponType { get; set; }
    }
    public class BuyCharaterRequest
    {
        public int AccountCode { get; set; }
        public int UseCash { get; set; }
        public int CharacterType { get; set; }
        public string characterName { get; set; }
    }
    public class BuyWeaponRequest
    {
        public int AccountCode { get; set; }
        public int UseCash { get; set; }
        public int WeaponType { get; set; }
    }
}
