using Microsoft.AspNetCore.Identity.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore;
using MMO_ApiServer_ASP.Model;
using System.Formats.Asn1;

namespace MMO_ApiServer_ASP.Context
{
    public class ApplicationDbContext : IdentityDbContext
    {
        public DbSet<Account> Accounts { get; set; }
        public DbSet<Player> Players { get; set; }

        public ApplicationDbContext(DbContextOptions<ApplicationDbContext> options) : base(options) { }

        protected override void OnModelCreating(ModelBuilder modelBuilder)
        {
            base.OnModelCreating(modelBuilder);

            // 기존 테이블과 엔터티 매핑
            modelBuilder.Entity<Account>().ToTable("Account");
            modelBuilder.Entity<Player>().ToTable("Player");
        }
    }
}
