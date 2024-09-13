#include "pch.h"

#include "DummyService.h"

#include "Session.h"
#include "DummySession.h"
#include "DummyPlayerInfo.h"
#include "DummyProto.pb.h"
#include "GamePacketHandler.h"
#include "SocketConfig.h"

DummyService::DummyService(std::string host, uint16 port, int32 _maxSessionCount) : Service(host, port, _maxSessionCount)
{
    _iocpHd = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, (u_long)0, 0);
    if (_iocpHd == INVALID_HANDLE_VALUE)
    {
        WSACleanup();
        CrashFunc(false);
    }
}

DummyService::~DummyService()
{
    std::cout << "~DummyService" << std::endl;
}

bool DummyService::Start()
{
    for (int i = 0; i < GetMaxSessionCount(); i++)
    {
        DummySessionRef session = std::static_pointer_cast<DummySession>(CreateSession());
        session->SetService(shared_from_this());
        session->Init();
        CrashFunc(SocketConfig::SetIoCompletionPort(session->GetSocket(), _iocpHd));
        if (!session->AsyncConnect())
            return false;
    }
    return true;
}

SessionRef DummyService::CreateSession()
{
    SessionRef session = std::make_shared<DummySession>(GetEndPoint());
    AddSessionRef(session);
    return session;
}

void DummyService::DummyTask()
{
    AsyncSession();
}

void DummyService::AsyncSession()
{
    std::chrono::system_clock::time_point current = std::chrono::system_clock::now();
    std::chrono::duration<double> sec = current - _timer;
    if (std::chrono::duration_cast<std::chrono::milliseconds>(sec).count() >= _timerDelay)
    {
        for (auto session : _sessions)
        {
            DummySessionRef dummySession = std::static_pointer_cast<DummySession>(session);
            DummyPlayerInfoRef info = dummySession->GetInfo();
            if (info == nullptr) continue;
            
            info->updatePosition();
            protocol::SMove pkt;
            pkt.set_is_monster(false);
            protocol::Position *position = new protocol::Position();
            position->set_x(info->GetPostion().X);
            position->set_y(info->GetPostion().Y);
            position->set_yaw(info->GetPostion().Yaw);
            pkt.set_allocated_position(position);
                
            SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(pkt, protocol::MessageCode::S_MOVE);
            session->AsyncWrite(sendBuffer);
        }
        _timer = current;
    }
}

void DummyService::DisConnectionAll()
{
    for (auto session : _sessions)
    {
        session->AsyncDisconnect();
    }
}
