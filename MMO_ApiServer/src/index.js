const express = require('express');
const app = express();
const port = 3000;
const {
  login,
  getAccount,
  getPlayers,
  getPlayer,
  updateAccount,
  buyCharacter,
  buyWeapon,
} = require('./service');

app.use(express.json());

app.post('/login', async (req, res) => {
  const body = req.body;
  const id = body.id;
  const pwd = body.pwd;

  const data = await login(id, pwd);
  console.log('result', data);
  res.send(data);
});

app.post('/getAccount', async (req, res) => {
  const body = req.body;
  const accountCode = body.accountCode;

  const data = await getAccount(accountCode);
  console.log('result', data);
  res.send(data);
});

app.post('/getPlayers', async (req, res) => {
  const body = req.body;
  const accountCode = body.accountCode;

  const data = await getPlayers(accountCode);
  console.log('result', data);
  res.send(data);
});

app.post('/getPlayer', async (req, res) => {
  const body = req.body;
  const accountCode = body.accountCode;
  const characterType = body.characterType;

  const data = await getPlayer(accountCode, characterType);
  console.log('result', data);
  res.send(data);
});

app.post('/updateAccount', async (req, res) => {
  const body = req.body;
  const accountCode = body.accountCode;
  const characterType = body.characterType;
  const weaponType = body.weaponType;

  const data = await updateAccount(accountCode, characterType, weaponType);
  console.log('result', data);
  res.send(data);
});

app.post('/buyCharacter', async (req, res) => {
  const body = req.body;
  const accountCode = body.accountCode;
  const useCash = parseInt(body.useCash);
  const characterType = body.characterType;
  const characterName = body.characterName;

  const data = await buyCharacter(accountCode, useCash, characterType, characterName);
  console.log('result', data);
  res.send(data);
});

app.post('/buyWeapon', async (req, res) => {
  const body = req.body;
  const accountCode = parseInt(body.accountCode);
  const useCash = parseInt(body.useCash);
  const weaponType = parseInt(body.weaponType);

  const data = await buyWeapon(accountCode, useCash, weaponType);
  console.log('result', data);
  res.send(data);
});

app.get('/', (req, res) => {
  res.send('deprecated api');
});

app.listen(port, () => {
  console.log(`GameApi Server listening on port ${port}`);
});
