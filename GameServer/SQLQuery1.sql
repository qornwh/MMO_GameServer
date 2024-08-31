DROP TABLE IF EXISTS Account
DROP TABLE IF EXISTS Player
DROP TABLE IF EXISTS InventoryEquip
DROP TABLE IF EXISTS InventoryEtc
-- DROP TABLE IF EXISTS IpLog
-- DROP TABLE IF EXISTS QuestInfo
-- DROP TABLE IF EXISTS QuestState
-- DROP TABLE IF EXISTS QuestKillState

CREATE TABLE Account
(
	accountCode int IDENTITY(1,1) NOT NULL,
	id varchar(10) NOT NULL PRIMARY KEY,
	pwd varchar(10) NOT NULL,
	cash int NOT NULL DEFAULT 10000,
	weaponOne int NOT NULL DEFAULT 1, -- ������ 1, ������ 0
	weaponTwo int NOT NULL DEFAULT 0,
	weaponThr int NOT NULL DEFAULT 0,
	curPlayerType int NOT NULL DEFAULT 0,
	curWeaponType int NOT NULL DEFAULT 0,
)

CREATE TABLE Player
(
	playerCode int IDENTITY(1,1) NOT NULL,
	name varchar(10) NOT NULL PRIMARY KEY,
	jobCode int NOT NULL,
	mapCode int NOT NULL,
	lv int NOT NULL DEFAULT 1,
	gold int NOT NULL DEFAULT 0,
    exp int NOT NULL DEFAULT 0,
	accountCode int NOT NULL,
)

CREATE TABLE InventoryEquip
(
	playerCode int NOT NULL,
	itemCode int NOT NULL,
	itemType int NOT NULL,
    itemUse int Not NULL DEFAULT 0,
)

CREATE TABLE InventoryEtc
(
    playerCode int NOT NULL,
    itemCode int NOT NULL,
    itemType int NOT NULL,
    itemCount int NOT NULL,
)

-- CREATE TABLE IpLog
-- (
-- 	accountCode int NOT NULL,
-- 	ip varchar(10) NOT NULL,
-- 	currentTime smalldatetime NOT NULL default getdate()
-- )
-- 
-- CREATE TABLE QuestInfo
-- (
-- 	questCode int NOT NULL PRIMARY KEY,
-- 	name varchar(20) NOT NULL,
-- 	type int NOT NULL,
-- )
-- 
-- CREATE TABLE QuestState
-- (
-- 	questStateId int NOT NULL PRIMARY KEY,
-- 	questState int NOT NULL,
-- 	questCode int NOT NULL,
-- 	playerCode int NOT NULL,
-- )
-- 
-- CREATE TABLE QuestKillState
-- (
-- 	questStateId int NOT NULL,
-- 	playerCode int NOT NULL,
-- 	monsterCode int NOT NULL,
-- 	monsterKill int NOT NULL
-- )

INSERT INTO Account (id, pwd, cash, weaponOne, weaponTwo, weaponThr, curPlayerType, curWeaponType) VALUES
('Gm', '1234', 10000, 1, 1, 1, 1, 1)

INSERT INTO Player (name, jobCode, mapCode, accountCode, gold, lv) VALUES
('GmPlayer1', 1, 1, 1, 1000, 1)

INSERT INTO InventoryEquip (playerCode, itemCode, itemType) VALUES
(1, 11, 1),
(1, 12, 2)

INSERT INTO InventoryEtc (playerCode, itemCode, itemType, itemCount) VALUES
(1, 1, 1, 10),
(1, 1, 1, 10)

-- INSERT INTO QuestInfo (questCode, name, type) VALUES
-- (1, '���� óġ', 2),
-- (2, '��� ����', 2)
-- 
