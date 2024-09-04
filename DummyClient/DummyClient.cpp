#include "pch.h"
#include "DummyService.h"
#include "SocketConfig.h"
#include "ThreadManager.h"

int main()
{
    // 게임 정보 초기화
    if (!SocketConfig::Init())
    {
        assert(-1);
    }

    std::string address = "127.0.0.1";
    uint16 port = 12128;

    // 일단 400까지는 연결이된다. 그러나 500부터는 안되는것 같다
    GThreadManager->CreateThread([&address, &port]()
        {
            DummyServiceRef service = std::make_shared<DummyService>(address, port, 100);
            if (!service->Start()) return;
            while (true)
            {
                service->task();
                service->DummyTask();
            }
        }
    );

    while (true)
    {
    }
    return 0;
}
