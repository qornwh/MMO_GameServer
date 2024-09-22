#pragma once
#include "Inventory.h"
#include "Types.h"

struct Mail
{
    Mail(int32 code, int32 read, int32 gold, int32 socket1, int32 socket1Type, int32 socket2, int32 socket2Type, wchar_t* title, wchar_t* message);
    Mail(const Mail& other);
    Mail(Mail&& other) noexcept;
    ~Mail();

    Mail& operator=(const Mail& other);
    Mail& operator=(Mail&& other) noexcept;

    static Mail EmptyEtcItem();

    int32 _code;
    int32 _read;
    int32 _gold;
    int32 _socket1;
    int32 _socket1Type;
    int32 _socket2;
    int32 _socket2Type;
    wchar_t _title[10];
    wchar_t _message[50];
};

class MailSystem
{
public:
    // TODO : 메일 리스트 갱신, 메일 읽기, 메일 아이템 받기, 메일 삭제, 삭제- 삭제안된 메일 구분, 삭제되면 바로 db가자, 메일 수신시 세션에 응답준다.
    MailSystem();
    ~MailSystem();

    void LoadMail(int32 playerCode); // 처음 로그인시 메일을 읽어온다.
    bool ReadMail(int32 mailCode, int32 playerCode); // 메일 읽음
    int32 CountEquipItemMail(int32 mailCode); // 미리 받을 아이템수 확인
    int32 CountEtcItemMail(int32 mailCode); // 미리 받을 아이템수(재료 공간) 확인
    bool ReciveItemMail(int32 mailCode, int32 playerCode); // 메일 아이템 받기
    bool ReciveItemEquipMail(int32 mailCode, int32 position); // 메일 아이템 받기
    bool ReciveItemEtcMail(int32 mailCode, int32 position); // 메일 아이템 받기
    void ReciveItemMailAll(int32 playerCode); // 메일 아이템 전부 받기
    bool RemoveMail(int32 mailCode, int32 playerCode); // 메일 삭제
    void RemoveItemEqipMail(int32 mailCode); // 메일 아이템 삭제
    void RemoveItemEtcMail(int32 mailCode); // 메일 아이템 삭제
    void RemoveMailAll(int32 playerCode); // 메일 전부 삭제
    void SendMail(Mail& mail, int32 playerCode); // 메일 보내기
    void SendEquipItemMail(Mail& mail, int32 playerCode, EquipItem& item); // 메일 아이템 장비 보내기
    void SendEtcItemMail(Mail& mail, int32 playerCode, EtcItem& item); // 메일 아이템 기타템 보내기
    void ReLoadMail(int32 playerCode); // 메일 다시 로드

    Map<int32, Mail>& GetMail() { return _mailList; }
    MapTuple<int32, int32, EquipItem>& GetMailEquip() { return _mailEquipList; }
    MapTuple<int32, int32, EtcItem>& GetMailEtc() { return _mailEtcList; }
    void SetPlayer(GameObjectInfoRef player);

private:
    Map<int32, Mail> _mailList;
    std::weak_ptr<GamePlayerInfo> _playerInfo;
    MapTuple<int32, int32, EquipItem> _mailEquipList; // 메일코드, 포지션이 키
    MapTuple<int32, int32, EtcItem> _mailEtcList; // 메일코드, 포지션이 키
    PriorityQueueDes<int32> _emptyMailPosList; // 메일 빈공간

    const int32 _itemMaxSize = 2;
};
