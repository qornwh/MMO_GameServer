using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using MMO_ApiServer_ASP.Context;
using MMO_ApiServer_ASP.Controllers.Service;
using MMO_ApiServer_ASP.Model;
using System.Text.Json.Nodes;

namespace MMO_ApiServer_ASP.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class LoginController : ControllerBase
    {
        private ApplicationDbContext _context;
        private AccountService _service;

        public LoginController(ApplicationDbContext context)
        {
            _context = context;
            _service = new AccountService(context);
        }

        [HttpGet("Test")]
        public string Test()
        {
            return "LoginController !!!";
        }

        [HttpPost("/login")]
        public AccountResponse LoginService([FromBody] LoginRequest result)
        {
            return _service.Login(result.Id, result.Pwd);
        }

        [HttpPost("/getAccount")]
        public AccountResponse GetAccount([FromBody] AccountRequest result)
        {
            return _service.GetAccount(result.AccountCode);
        }

        [HttpPost("/getPlayers")]
        public PlayersRespons GetPlayers([FromBody] AccountRequest result)
        {
            return _service.GetPlayers(result.AccountCode);
        }

        [HttpPost("/getPlayer")]
        public PlayerRespons GetPlayer([FromBody] PlayerRequest result)
        {
            return _service.GetPlayer(result.AccountCode, result.CharacterType);
        }

        [HttpPost("/updateAccount")]
        public AccountResponse UpdateAccount([FromBody] UpdateAccountRequest result)
        {
            return _service.UpdateAccount(result.AccountCode, result.CharacterType, result.WeaponType);
        }

        [HttpPost("/buyCharacter")]
        public AccountResponse BuyCharacter([FromBody] BuyCharaterRequest result)
        {
            return _service.BuyCharacter(result.AccountCode, result.UseCash, result.CharacterType, result.CharacterName);
        }

        [HttpPost("/buyWeapon")]
        public AccountResponse BuyWeapon([FromBody] BuyWeaponRequest result)
        {
            return _service.BuyWeapon(result.AccountCode, result.UseCash, result.WeaponType);
        }
    }
}
