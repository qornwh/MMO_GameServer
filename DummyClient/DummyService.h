#pragma once
#include "Service.h"

class DummyService : public Service
{
public:
    DummyService(std::string host, uint16 port, int32 _maxSessionCount = 20);
    ~DummyService();

    bool Start() override;
    SessionRef CreateSession() override;
    void DummyTask();

private:
    void AsyncSession();

private:
    uint8 _tick = 0;
    std::chrono::system_clock::time_point _timer;
    int32 _timerDelay = 1000;
};
