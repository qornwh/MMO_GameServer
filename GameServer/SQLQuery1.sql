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
    position int Not NULL,
)

-- ��Ÿ������
CREATE TABLE InventoryEtc
(
    playerCode int NOT NULL,
    itemCode int NOT NULL,
    itemType int NOT NULL,
    itemCount int NOT NULL,
    position int Not NULL,
)

--  ģ�� �ý���
CREATE TABLE Friend
(
    playerCode int NOT NULL,
    friendCode int NOT NULL
)

-- ���� ���� ����
-- socket1 = MailEquip, MailEtc�� positionüũ��, 
-- socket1 = -1 ���� ����
-- socket1 = 0  �̹� ����
-- socket1 = 1  ���� ���� ���
CREATE TABLE Mail
(
    mailCode int IDENTITY(1,1) NOT NULL,
    playerCode int NOT NULL,
    title varchar(50),
    message varchar(50),
    gold int NOT NULL DEFAULT 0,
    readCheck int NOT NULL DEFAULT 1,
    socket1 int NOT NULL,
    socket1Type int NOT NULL,
    socket2 int NOT NULL,
    socket2Type int NOT NULL
)

CREATE TABLE MailEquipItem
(
    mailCode int NOT NULL,
    uniqueId UNIQUEIDENTIFIER DEFAULT NEWID(),
    playerCode int NOT NULL,
    itemCode int NOT NULL,
    equipType int NOT NULL,
    attack int NOT NULL,
    speed int NOT NULL,
    position int NOT NULL,
    isRemove int NOT NULL DEFAULT 0
)

CREATE TABLE MailEtcItem
(
    mailCode int NOT NULL,
    uniqueId UNIQUEIDENTIFIER DEFAULT NEWID(),
    playerCode int NOT NULL,
    itemCode int NOT NULL,
    itemType int NOT NULL,
    itemCount int NOT NULL,
    position int NOT NULL,
    isRemove int NOT NULL DEFAULT 0
)

-- ������ ���ϵ�
CREATE TABLE RemoveMail
(
    mailCode int NOT NULL,
    playerCode int NOT NULL,
    title varchar(50),
    message varchar(50),
    gold int NOT NULL DEFAULT 0,
    readCheck int NOT NULL DEFAULT 1,
    socket1 int NOT NULL,
    socket1Type int NOT NULL,
    socket2 int NOT NULL,
    socket2Type int NOT NULL
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

INSERT INTO InventoryEquip (playerCode, itemCode, equipType, attack, speed, isEquip, position) VALUES
(1, 11, 1, 100, 0, 0, 2),
(1, 12, 2, 0, 100, 0, 4),
(2, 11, 1, 100, 0, 0, 6),
(2, 12, 2, 0, 100, 0, 7)
    
INSERT INTO InventoryEtc (playerCode, itemCode, itemType, itemCount, position) VALUES
(1, 1, 1, 10, 1),
(1, 2, 1, 11, 5),
(2, 1, 1, 10, 2),
(2, 2, 1, 11, 3)

INSERT INTO Friend (playerCode, friendCode) VALUES
(1, 2),
(2, 1)

--
INSERT INTO Mail (playerCode, title, message, gold, readCheck, socket1, socket1Type, socket2, socket2Type) VALUES
(1, 'test1', 'mmmm1', 100, 1, 1, 1, 1, 1),
(1, 'test2', 'mmmmmmmmmmm2', 100, 1, 1, 1, 1, 1),
(1, 'test3', 'mmmmmmm3', 100, 1, 1, 1, 1, 1),
(1, 'test4', 'm4', 100, 1, 1, 1, 1, 2),
(1, 'test5', 'mm5', 100, 1, 1, 1, 1, 2),
(1, 'test6', 'mm6', 100, 1, 1, 1, 1, 2)
    
INSERT INTO MailEquipItem (mailCode, playerCode, itemCode, equipType, attack, speed, position) VALUES
(1, 1, 11, 1, 10, 0, 1),
(2, 1, 11, 1, 11, 0, 1),
(3, 1, 11, 1, 13, 0, 1),
(4, 1, 11, 1, 14, 0, 1),
(5, 1, 11, 1, 15, 0, 1),
(6, 1, 11, 1, 16, 0, 1),
(1, 1, 12, 2, 0, 10, 2),
(2, 1, 12, 2, 0, 20, 2),
(3, 1, 12, 2, 0, 20, 2)
    
INSERT INTO MailEtcItem (mailCode, playerCode, itemCode, itemType, itemCount, position) VALUES
(4, 1, 1, 1, 10, 2),
(5, 1, 2, 1, 20, 2),
(6, 1, 1, 1, 10, 2)

--
