#pragma once
#include "Session.h"

class DummySession : public Session
{
public:
    DummySession(EndPointUtil& ep);
    ~DummySession();

    void SetId(int32 id);
    int32 GetId() { return _id; }
    void OnWait();
    void AsyncLoad();
    bool AsyncConnect() override;
    void OnConnect() override;
    DummyPlayerInfoRef GetInfo() { return _playerInfo; }

private:
    int32 _id;
    DummyPlayerInfoRef _playerInfo;
    int32 _roomIdx = 0;
    
    OverlappedSocket _connOLS;
};
