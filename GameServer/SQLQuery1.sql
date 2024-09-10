DROP TABLE IF EXISTS Account
DROP TABLE IF EXISTS Player
DROP TABLE IF EXISTS InventoryEquip
DROP TABLE IF EXISTS InventoryEtc
DROP TABLE IF EXISTS IpLog
DROP TABLE IF EXISTS Friend
DROP TABLE IF EXISTS Mail
DROP TABLE IF EXISTS MailEquipItem
DROP TABLE IF EXISTS MailEtcItem
DROP TABLE IF EXISTS RemoveMail

-- ���� ����
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

-- �÷��̾�
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

-- ��������
CREATE TABLE InventoryEquip
(
    equipUniqueId UNIQUEIDENTIFIER DEFAULT NEWID(),
    playerCode int NOT NULL,
    itemCode int NOT NULL,
    equipType int NOT NULL,
    attack int Not NULL,
    speed int Not NULL,
    isEquip int Not NULL DEFAULT 0,
)

-- ��Ÿ������
CREATE TABLE InventoryEtc
(
    playerCode int NOT NULL,
    itemCode int NOT NULL,
    itemType int NOT NULL,
    itemCount int NOT NULL,
)

--  ģ�� �ý���
CREATE TABLE Friend
(
    playerCode int NOT NULL,
    friendCode int NOT NULL
)

-- ���� ���� ����
CREATE TABLE Mail
(
    mailCode int  IDENTITY(1,1) NOT NULL,
    title varchar(50),
    message varchar(50),
    gold int NOT NULL DEFAULT 0,
    readCheck int NOT NULL DEFAULT 1,
    itemCode1 int NOT NULL ,
    itemCode2 int NOT NULL
)

CREATE TABLE MailEquipItem
(
    equipUniqueId int IDENTITY(1,1) NOT NULL,
    mailCode int NOT NULL,
    useCheck int NOT NULL DEFAULT 1,
    itemCode int NOT NULL,
    itemType int NOT NULL
)

CREATE TABLE MailEtcItem
(
    mailCode int  IDENTITY(1,1) NOT NULL,
    useCheck int NOT NULL DEFAULT 1,
    itemCode int NOT NULL,
    itemType int NOT NULL,
    itemCount int NOT NULL
)

-- ������ ���ϵ�
CREATE TABLE RemoveMail
(
    MailCode int NOT NULL,
    title varchar(50),
    message varchar(50),
)

-- �α� ���
CREATE TABLE IpLog
(
    accountCode int NOT NULL,
    ip varchar(10) NOT NULL,
    currentTime smalldatetime NOT NULL default getdate()
)

    INSERT INTO Account (id, pwd, cash, weaponOne, weaponTwo, weaponThr, curPlayerType, curWeaponType) VALUES
('Gm', '1234', 10000, 1, 1, 1, 1, 1),
('Gm2', '1234', 10000, 1, 1, 1, 1, 1)

INSERT INTO Player (name, jobCode, mapCode, accountCode, gold, lv) VALUES
('GmPlayer1', 1, 1, 1, 1000, 1),
('GmPlayer2', 1, 1, 2, 1000, 1)

INSERT INTO InventoryEquip (playerCode, itemCode, equipType, attack, speed, isEquip) VALUES
(1, 11, 1, 100, 0, 1),
(1, 12, 2, 0, 100, 1),
(2, 11, 1, 100, 0, 1),
(2, 12, 2, 0, 100, 1)
    
INSERT INTO InventoryEtc (playerCode, itemCode, itemType, itemCount) VALUES
(1, 1, 1, 10),
(1, 2, 1, 11),
(2, 1, 1, 10),
(2, 2, 1, 11)

INSERT INTO Friend (playerCode, friendCode) VALUES
(1, 2),
(2, 1)

