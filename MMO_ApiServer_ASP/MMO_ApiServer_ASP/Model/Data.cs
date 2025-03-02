using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace MMO_ApiServer_ASP.Model
{
    public class Data
    {
        [Key]
        [Column("accountCode")]
        public int accountCode { get; set; }

        [Column("id")]
        public string? id { get; set; }

        [Column("pwd")]
        public string? pwd { get; set; }

        [Column("cash")]
        public int cash { get; set; }

        [Column("weaponOne")]
        public int weaponOne { get; set; }

        [Column("weaponTwo")]
        public int weaponTwo { get; set; }

        [Column("weaponThr")]
        public int weaponThr { get; set; }

        [Column("curPlayerType")]
        public int curPlayerType { get; set; }

        [Column("curWeaponType")]
        public int curWeaponType { get; set; }
    }

    public class Player
    {
        [Key]
        [Column("playerCode")]
        public int playerCode { get; set; }

        [Column("name")]
        public string name { get; set; }

        [Column("jobCode")]
        public int jobCode { get; set; }

        [Column("mapCode")]
        public int mapCode { get; set; }

        [Column("lv")]
        public int lv { get; set; }

        [Column("exp")]
        public int exp { get; set; }

        [Column("gold")]
        public int gold { get; set; }

        [Column("accountCode")]
        public int accountCode { get; set; }
    }
}
