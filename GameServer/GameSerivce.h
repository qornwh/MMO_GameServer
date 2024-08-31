#pragma once
#include "Service.h"
#include "Session.h"

class GameService : public Service
{
public:
    GameService(uint16 port, int32 _maxSessionCount = 200);
    GameService(std::string host, uint16 port, int32 _maxSessionCount = 200);
    void RegistAccept(OverlappedSocket* overlappedPtr) override;
    
public:
    ~GameService();

    bool Start();
    void BroadCast(SendBufferRef sendBuffer) override;
    void ReleaseSessionMesssage(SessionRef session) override;
    SessionRef CreateSession() override;
};
