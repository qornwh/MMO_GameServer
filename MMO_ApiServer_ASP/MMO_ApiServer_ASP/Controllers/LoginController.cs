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
        public JsonObject LoginService([FromBody] LoginRequest result)
        {
            JsonObject obj = new JsonObject();
            obj["ret"] = 0;
            _service.Login(result.Id, result.Pwd, obj);
            return obj;
        }

        [HttpPost("/getAccount")]
        public JsonObject GetAccount([FromBody] AccountRequest result)
        {
            JsonObject obj = new JsonObject();
            obj["ret"] = 0;
            _service.GetAccount(result.AccountCode, obj);
            return obj;
        }

        [HttpPost("/getPlayers")]
        public JsonObject GetPlayers([FromBody] AccountRequest result)
        {
            JsonObject obj = new JsonObject();
            obj["ret"] = 0;
            _service.GetPlayers(result.AccountCode, obj);
            return obj;
        }

        [HttpPost("/getPlayer")]
        public JsonObject GetPlayer([FromBody] PlayerRequest result)
        {
            JsonObject obj = new JsonObject();
            obj["ret"] = 0;
            _service.GetPlayer(result.AccountCode, result.CharacterType, obj);
            return obj;
        }

        [HttpPost("/updateAccount")]
        public JsonObject UpdateAccount([FromBody] UpdateAccountRequest result)
        {
            JsonObject obj = new JsonObject();
            obj["ret"] = 0;
            _service.UpdateAccount(result.AccountCode, result.CharacterType, result.WeaponType, obj);
            return obj;
        }

        [HttpPost("/buyCharacter")]
        public JsonObject BuyCharacter([FromBody] BuyCharaterRequest result)
        {
            JsonObject obj = new JsonObject();
            obj["ret"] = 0;
            _service.BuyCharacter(result.AccountCode, result.UseCash, result.CharacterType, result.CharacterName, obj);
            return obj;
        }

        [HttpPost("/buyWeapon")]
        public JsonObject BuyWeapon([FromBody] BuyWeaponRequest result)
        {
            JsonObject obj = new JsonObject();
            obj["ret"] = 0;
            _service.BuyWeapon(result.AccountCode, result.UseCash, result.WeaponType, obj);
            return obj;
        }
    }
}
