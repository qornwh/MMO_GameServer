#pragma once
#include "Session.h"
#include "PacketHeader.h"

class GameSession : public Session
{
public:
    GameSession(EndPointUtil& ep);
    ~GameSession();

    void LoginHandler(BYTE* buffer, PacketHeader* header, int32 offset);
    void UpdateAccountHandler(BYTE* buffer, PacketHeader* header, int32 offset);
    void BuyCharaterHandler(BYTE* buffer, PacketHeader* header, int32 offset);
    void BuyWeaponHandler(BYTE* buffer, PacketHeader* header, int32 offset);
    void LoadHandler(BYTE* buffer, PacketHeader* header, int32 offset);
    void MoveHandler(BYTE* buffer, PacketHeader* header, int32 offset);
    void ChatHandler(BYTE* buffer, PacketHeader* header, int32 offset);
    void AttackHandler(BYTE* buffer, PacketHeader* header, int32 offset);
    void ChangeRoomHandler(BYTE* buffer, PacketHeader* header, int32 offset);
    void DamegaHandler(BYTE* buffer, PacketHeader* header, int32 offset);
    void PlayerAimHandler(BYTE* buffer, PacketHeader* header, int32 offset);
    void PlayerJumpHandler(BYTE* buffer, PacketHeader* header, int32 offset);
    void SellItemsHandler(BYTE* buffer, PacketHeader* header, int32 offset);
    
    void HandlePacket(BYTE* buffer, PacketHeader* header);
    int32 OnRecv(BYTE* buffer, int32 len) override;
    void CreatePlayerInfo(int32 jobCode, int32 weaponCode, int32 lv);
    std::shared_ptr<class GamePlayerInfo> GetPlayer();
    uint16 GetSessionId() { return _sessionId; }
    void SetRoomId(uint32 id) { _roomId = id; }
    int32 GetRoomId() { return _roomId; }
    void AddExp(int32 exp);
    void DropItem(std::shared_ptr<GameMosterInfo> monster);
    int32 GetAccountCode() { return _accountCode; }

private:
    std::shared_ptr<class GamePlayerInfo> _player;
    int32 _roomId = -1;
    uint16 _sessionId = -1;
    int32 _accountCode = -1;
    int32 _playerCode = -1;
    int32 _jobCode = -1;
    int32 _weaponCode = -1;
    String _logId = "";

    // dummy 세션 관련
    void D_LoadHandler(BYTE* buffer, PacketHeader* header, int32 offset);
    
private:
    bool _dummy = false;
    int32 _d_jobCode = 1;
    int32 _d_weaponCode = 1;
};
