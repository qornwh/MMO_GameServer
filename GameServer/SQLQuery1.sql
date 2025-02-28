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

CREATE TABLE Account
(
    accountCode int IDENTITY(1,1) NOT NULL,
    id varchar(10) NOT NULL PRIMARY KEY,
    pwd varchar(10) NOT NULL,
    cash int NOT NULL DEFAULT 10000,
    weaponOne int NOT NULL DEFAULT 1, -- 보유 1, 비보유 0
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
    --equipUniqueId UNIQUEIDENTIFIER DEFAULT NEWID(),
    uniqueId varchar(36) NOT NULL PRIMARY KEY,
    playerCode int NOT NULL,
    itemCode int NOT NULL,
    equipType int NOT NULL,
    attack int Not NULL,
    speed int Not NULL,
    equipPos int Not NULL,
    invenPos int Not NULL,
)

CREATE TABLE InventoryEtc
(
    playerCode int NOT NULL,
    itemCode int NOT NULL,
    itemType int NOT NULL,
    itemCount int NOT NULL,
    invenPos int Not NULL,
)

CREATE TABLE Friend
(
    playerCode int NOT NULL,
    friendCode int NOT NULL
)

-- socket1 = MailEquip, MailEtc position, 
-- socket1 = -1 첨부 x
-- socket1 = 0  첨부 수령 o
-- socket1 = 1  첨부 수령 x
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
    uniqueId varchar(36) NOT NULL,
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
    playerCode int NOT NULL,
    itemCode int NOT NULL,
    itemType int NOT NULL,
    itemCount int NOT NULL,
    position int NOT NULL,
    isRemove int NOT NULL DEFAULT 0
)

--CREATE TABLE RemoveMail
--(
--    mailCode int NOT NULL,
--    playerCode int NOT NULL,
--    title varchar(50),
--    message varchar(50),
--    gold int NOT NULL DEFAULT 0,
--    readCheck int NOT NULL DEFAULT 1,
--    socket1 int NOT NULL,
--    socket1Type int NOT NULL,
--    socket2 int NOT NULL,
--    socket2Type int NOT NULL
--)

-- CREATE TABLE IpLog
-- (
--     accountCode int NOT NULL,
--     ip varchar(10) NOT NULL,
--     currentTime smalldatetime NOT NULL default getdate()
-- )

CREATE INDEX acc ON Account (id ASC)

-- defatult 더미 row
INSERT INTO Account (id, pwd, cash, weaponOne, weaponTwo, weaponThr, curPlayerType, curWeaponType) VALUES
('Gm', '1234', 10000, 1, 1, 1, 1, 1),
('Gm2', '1234', 10000, 1, 1, 1, 1, 1)

INSERT INTO Player (name, jobCode, mapCode, accountCode, gold, lv) VALUES
('GmPlayer1', 1, 1, 1, 1000, 1),
('GmPlayer2', 1, 1, 2, 1000, 1)

INSERT INTO InventoryEquip (uniqueId, playerCode, itemCode, equipType, attack, speed, equipPos, invenPos) VALUES
('32566CDA-203C-4A25-B618-21305C30C60B', 1, 11, 1, 100, 0, 0, 2),
('F84C9EE6-3D61-4413-AF3E-A5A0816ECD0B', 1, 12, 2, 0, 100, 0, 4),
('5698D92C-7CEF-49CD-BBE8-7327A02376C5', 2, 11, 1, 100, 0, 0, 6),
('C98E7F91-079E-4D86-BD1D-670F4D226504', 2, 12, 2, 0, 100, 0, 7)
    
INSERT INTO InventoryEtc (playerCode, itemCode, itemType, itemCount, invenPos) VALUES
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
    
INSERT INTO MailEquipItem (mailCode, uniqueId, playerCode, itemCode, equipType, attack, speed, position) VALUES
(1, 'BBC6DE7C-91A8-45DD-B7A9-9AB431A4BDB4', 1, 11, 1, 10, 0, 1),
(2, 'CE884299-D1ED-4F27-B0AD-1D414D8A5A36', 1, 11, 1, 11, 0, 1),
(3, '7CDD68E0-CAB6-4D06-A329-7BB331BA2EBB', 1, 11, 1, 13, 0, 1),
(4, '05613B56-4C8A-4977-BAA6-ACF92F123443', 1, 11, 1, 14, 0, 1),
(5, '7FF97BA3-9C5B-4E51-948D-ED13C7D57802', 1, 11, 1, 15, 0, 1),
(6, '1B4D1AEB-0CAA-4216-8B57-C27C36647A5A', 1, 11, 1, 16, 0, 1),
(1, '2FA3382A-A02F-4D72-807D-BFC5C034FB18', 1, 12, 2, 0, 10, 2),
(2, '99AEF1ED-A848-4B4D-B371-FFAB71CF5241', 1, 12, 2, 0, 20, 2),
(3, 'F1D60484-1548-4866-984A-CBE1A728BCB7', 1, 12, 2, 0, 20, 2)
    
INSERT INTO MailEtcItem (mailCode, playerCode, itemCode, itemType, itemCount, position) VALUES
(4, 1, 1, 1, 10, 2),
(5, 1, 2, 1, 20, 2),
(6, 1, 1, 1, 10, 2)
--
