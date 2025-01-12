const { popConn, pushConn } = require('./dbPool');

async function login(id, pwd) {
  let result = await getAccountInfo(id);

  if (result.length > 0) {
    result = result[0];
    if (result.pwd === pwd) {
      result.ret = 1;
    } else {
      result = { ret: 0 };
    }
  } else {
    // 계정 생성
    result = { ret: 0 };
    if (pwd.length < 10) {
      result = await addAccountInfo(id, pwd);
      if (result == 1) result = { ret: 2 };
    }
  }
  return result;
}

async function getAccount(accountCode) {
  let result = await getAccountInfoCode(accountCode);

  if (result.length > 0) {
    result = result[0];
    result.ret = 1;
  } else {
    result = { ret: 0 };
  }
  return result;
}

async function getPlayers(accountCode) {
  let result = await getAccountCharacterInfoList(accountCode);

  if (result.length > 0) {
    result = { characters: result, ret: 1 };
  } else {
    result = { ret: 0 };
  }
  return result;
}

async function getPlayer(accountCode, characterType) {
  let result = await getAccountCharacterInfo(accountCode, characterType);

  if (result.length > 0) {
    result = { character: result, ret: 1 };
  } else {
    result = { ret: 0 };
  }
  return result;
}

async function updateAccount(accountCode, characterType, weaponType) {
  let result = await getAccountInfoCode(accountCode);

  if (result.length > 0) {
    result = result[0];
    if (result.curPlayerType != characterType) {
      let characters = await getAccountCharacterInfoList(accountCode);
      if (characterType > 0) {
        characters.forEach((character) => {
          if (characterType == character.jobCode) {
            result.curPlayerType = characterType;
          }
        });
      } else if (characterType == 0) {
        characterType = result.curPlayerType;
      }
    }

    if (result.curWeaponType != weaponType) {
      if (result.weaponOne == 1 && weaponType == 1) {
        result.curWeaponType = 1;
      } else if (result.weaponTwo == 1 && weaponType == 2) {
        result.curWeaponType = 2;
      } else if (result.weaponThr == 1 && weaponType == 3) {
        result.curWeaponType = 3;
      } else if (weaponType == 0) {
        weaponType = result.curWeaponType;
      }
    }

    if (result.curPlayerType == characterType && result.curWeaponType == weaponType) {
      await updateAccountInfo(
        result.cash,
        result.curPlayerType,
        result.curWeaponType,
        result.weaponOne,
        result.weaponTwo,
        result.weaponThr,
        accountCode,
      );
      result = { ...result, ret: 1 };
    } else {
      result = { ret: 0 };
    }
  } else {
    result = { ret: 0 };
  }
  return result;
}

async function buyCharacter(accountCode, useCash, characterType, characterName) {
  let result = await getAccountInfoCode(accountCode);

  if (result.length > 0) {
    result = result[0];
    const cash = result.cash;

    if (cash >= useCash) {
      let characters = await getAccountCharacterInfo(accountCode, characterType);
      if (characters.length == 0) {
        const rows = await addAccountCharacter(accountCode, characterType, characterName);
        if (rows == 1) {
          result.cash -= useCash;
          result.ret = 1;
          return result;
        }
      }
    }
  }
  result = { ret: 0 };
  return result;
}

async function buyWeapon(accountCode, useCash, weaponType) {
  let result = await getAccountInfoCode(accountCode);

  if (result.length > 0 && result[0].cash >= useCash) {
    let isUpdate = 0;
    result = result[0];
    result.cash -= useCash;
    if (result.weaponOne == 0 && weaponType == 1) {
      isUpdate = await updateAccountWeapon(accountCode, result.cash, weaponType);
      result.weaponOne = 1;
    } else if (result.weaponTwo == 0 && weaponType == 2) {
      isUpdate = await updateAccountWeapon(accountCode, result.cash, weaponType);
      result.weaponTwo = 1;
    } else if (result.weaponThr == 0 && weaponType == 3) {
      isUpdate = await updateAccountWeapon(accountCode, result.cash, weaponType);
      result.weaponThr = 1;
    }

    if (isUpdate == 1) {
      await updateAccountInfo(
        result.cash,
        result.curPlayerType,
        result.curWeaponType,
        result.weaponOne,
        result.weaponTwo,
        result.weaponThr,
        accountCode,
      );
    }
    if (isUpdate == 1) result = { ...result, ret: 1 };
    else result = { ret: 0 };
  } else {
    result = { ret: 0 };
  }

  return result;
}

async function addAccountInfo(id, pwd) {
  try {
    const query = `INSERT INTO Account (id, pwd, cash) VALUES ('${id}', '${pwd}', 10000)`;
    const conn = await popConn();
    let result = await conn.request().query(query);
    pushConn(conn);
    return result.rowsAffected.length;
  } catch (err) {
    console.error('', err);
    throw err;
  }
}

async function getAccountInfo(id) {
  try {
    const query = `SELECT accountCode, id, pwd, cash, curPlayerType, curWeaponType, weaponOne, weaponTwo, weaponThr FROM Account WHERE id = '${id}'`;
    const conn = await popConn();
    let result = await conn.request().query(query);
    pushConn(conn);
    return result.recordset;
  } catch (err) {
    console.error('', err);
    throw err;
  }
}

async function getAccountInfoCode(accountCode) {
  try {
    const query = `SELECT cash, curPlayerType, curWeaponType, weaponOne, weaponTwo, weaponThr FROM Account WHERE accountCode = '${accountCode}'`;
    const conn = await popConn();
    let result = await conn.request().query(query);
    pushConn(conn);
    return result.recordset;
  } catch (err) {
    console.error('', err);
    throw err;
  }
}

async function getAccountCharacterInfoList(accountCode) {
  try {
    const query = `SELECT playerCode, name, jobCode, mapCode, gold, lv, exp FROM Player WHERE accountCode = '${accountCode}'`;
    const conn = await popConn();
    let result = await conn.request().query(query);
    pushConn(conn);
    return result.recordset;
  } catch (err) {
    console.error('', err);
    throw err;
  }
}

async function updateAccountInfo(
  cash,
  curPlayerType,
  curWeaponType,
  weaponOne,
  weaponTwo,
  weaponThr,
  accountCode,
) {
  try {
    const query = `UPDATE Account SET cash = '${cash}', curPlayerType = '${curPlayerType}', curWeaponType = '${curWeaponType}', weaponOne = '${weaponOne}', weaponTwo = '${weaponTwo}', weaponThr = '${weaponThr}' WHERE accountCode = '${accountCode}'`;
    const conn = await popConn();
    let result = await conn.request().query(query);
    pushConn(conn);
    return result.rowsAffected.length;
  } catch (err) {
    console.error('', err);
    throw err;
  }
}

async function addAccountCharacter(accountCode, jobType, characterName) {
  try {
    const query = `INSERT INTO Player (name, jobCode, mapCode, accountCode, gold, lv) OUTPUT inserted.playerCode VALUES ('${characterName}', '${jobType}', 1, '${accountCode}', 0, 1)`;
    const conn = await popConn();
    let result = await conn.request().query(query);
    pushConn(conn);
    return result.rowsAffected.length;
  } catch (err) {
    console.error('', err);
    throw err;
  }
}

async function getAccountCharacterInfo(accountCode, jobType) {
  try {
    const query = `SELECT playerCode, name, jobCode, mapCode, gold, lv, exp FROM Player WHERE accountCode = '${accountCode}' AND jobCode = '${jobType}'`;
    const conn = await popConn();
    let result = await conn.request().query(query);
    pushConn(conn);
    return result.recordset;
  } catch (err) {
    console.error('', err);
    throw err;
  }
}

async function updateAccountWeapon(accountCode, cash, weaponType) {
  try {
    let query = '';
    if (weaponType == 1) {
      query = `UPDATE Account SET cash = '${cash}', weaponOne = 1 WHERE accountCode = '${accountCode}'`;
    } else if (weaponType == 2) {
      query = `UPDATE Account SET cash = '${cash}', weaponTwo = 1 WHERE accountCode = '${accountCode}'`;
    } else {
      query = `UPDATE Account SET cash = '${cash}', weaponThr = 1 WHERE accountCode = '${accountCode}'`;
    }
    const conn = await popConn();
    if (query !== '') {
      let result = await conn.request().query(query);
      pushConn(conn);
      return result.rowsAffected.length;
    }
    return 0;
  } catch (err) {
    console.error('', err);
    throw err;
  }
}

module.exports = {
  login: login,
  getAccount: getAccount,
  getPlayers: getPlayers,
  getPlayer: getPlayer,
  updateAccount: updateAccount,
  buyCharacter: buyCharacter,
  buyWeapon: buyWeapon,
};
