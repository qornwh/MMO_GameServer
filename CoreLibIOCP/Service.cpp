#include "pch.h"
#include "Service.h"
#include "OverlappedSocket.h"
#include "SocketConfig.h"
#include "Session.h"

Service::Service(uint16 port, int32 maxSessionCount) :
    _maxSessionCount(maxSessionCount)
{
    _ep.port = port;
}

Service::Service(String host, uint16 port, int32 maxSessionCount) :
    _maxSessionCount(maxSessionCount)
{
    _ep.host = host;
    _ep.port = port;
}

Service::~Service()
{
    CloseHandle(_iocpHd);
}

void Service::Init()
{
    // iocp 초기화
    _iocpHd = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, (u_long)0, 0);
    if (_iocpHd == INVALID_HANDLE_VALUE)
    {
        WSACleanup();
        assert(-1);
    }

    // 서버 소켓 생성(리슨용)
    _serverSocket = SocketConfig::CreateSocket();
    if (_serverSocket == INVALID_SOCKET)
    {
        WSACleanup();
        assert(-1);
    }

    // iocp 등록
    if (!SocketConfig::SetIoCompletionPort(_serverSocket, _iocpHd))
    {
        WSACleanup();
        assert(-1);
    }

    if (SocketConfig::SetReuseAddress(_serverSocket, 1))
    {
        WSACleanup();
        return;
    }

    // bind
    if (!SocketConfig::SocketBind(_serverSocket, _ep.port))
    {
        WSACleanup();
        assert(-1);
    }

    // listen
    if (!SocketConfig::SocketListen(_serverSocket))
    {
        WSACleanup();
        assert(-1);
    }

    ZeroMemory(&_overlapped, sizeof(_overlapped));
}

bool Service::Start()
{
    Init();
    for (int i = 0; i < GetMaxSessionCount(); i++)
    {
        OverlappedSocket* overlappedPtr = new OverlappedSocket();
        overlappedPtr->SetType(0); // 초기 설정
        RegistAccept(overlappedPtr);
    }
    return true;
}

SessionRef Service::CreateSession()
{
    SessionRef session = std::make_shared<Session>(_ep);
    return session;
}

void Service::AddSessionRef(SessionRef session)
{
    WriteLockGuard wl(lock, "writeLock");
    _sessions.insert(session);
    _sessionCount++;
}

void Service::BroadCast(SendBufferRef sendBuffer)
{
    ReadLockGuard rl(lock, "BroadCast");
    for (auto session : _sessions)
    {
        if (session->IsConnected())
        {
            session->AsyncWrite(sendBuffer);
        }
    }
}

void Service::task()
{
    DWORD numOfBytes = 0;
    ULONG_PTR key = 0;
    OverlappedSocket* overlappedPtr = nullptr;

    if (GetQueuedCompletionStatus(_iocpHd, &numOfBytes, &key, reinterpret_cast<LPOVERLAPPED*>(&overlappedPtr), 10))
    {
        if (overlappedPtr == nullptr)
            return;
        int32 type = overlappedPtr->GetType();
        if (type == OverlappedSocket::Type::ACCP)
        {
            Accept(overlappedPtr);
        }
        else if (type == OverlappedSocket::Type::READ)
        {
            SessionRef session = overlappedPtr->GetSession();
            if (session != nullptr)
            {
                session->OnRead(numOfBytes);
            }
        }
        else if (type == OverlappedSocket::Type::SEND)
        {
            SessionRef session = overlappedPtr->GetSession();
            if (session != nullptr)
            {
                session->OnWrite(numOfBytes);
            }
        }
        else if (type == OverlappedSocket::Type::CONN)
        {
            SessionRef session = overlappedPtr->GetSession();
            if (session != nullptr)
            {
                session->OnConnect();
            }
        }
        else if (type == OverlappedSocket::Type::DISC)
        {
            SessionRef session = overlappedPtr->GetSession();
            if (session != nullptr)
            {
                session->OnDisconnect();
            }
        }
    }
    else
    {
        int errorCode = WSAGetLastError();
        if (errorCode != WAIT_TIMEOUT)
        {
            if (errorCode == ERROR_NETNAME_DELETED)
            {
                // 이미 종료됨
                return;
            }
            ErrorCode(errorCode);
            assert(-1);
        }
    }
}

void Service::RegistAccept(OverlappedSocket* overlappedPtr)
{
    SessionRef session = std::make_shared<Session>(_ep);
    session->Init();
    session->SetService(shared_from_this());
    overlappedPtr->SetSession(session);
    SocketAcceptRegister(overlappedPtr);
    CrashFunc(SocketConfig::SetIoCompletionPort(session->GetSocket(), _iocpHd));
}

void Service::Accept(OverlappedSocket* overlappedPtr)
{
    std::shared_ptr<Session> session = overlappedPtr->GetSession();
    CrashFunc(session != nullptr);

    if (SocketConfig::SetUpdateAcceptSocket(session->GetSocket(), _serverSocket))
    {
        RegistAccept(overlappedPtr);
        return;
    }

    SOCKADDR_IN sockAddress;
    int sizeOfSockAddr = sizeof(sockAddress);
    if (SOCKET_ERROR == getpeername(session->GetSocket(), OUT reinterpret_cast<SOCKADDR*>(&sockAddress), &sizeOfSockAddr))
    {
        RegistAccept(overlappedPtr);
        return;
    }

    session->OnAccept();
    RegistAccept(overlappedPtr);
}

void Service::ReleaseSession(SessionRef session)
{
    {
        WriteLockGuard wl(lock, "writeLock");
        _sessions.erase(session);
        _sessionCount--;
    }

    ReleaseSessionMesssage(session);
}

void Service::SocketAcceptRegister(OverlappedSocket* overlappedPtr)
{
    std::shared_ptr<Session> session = overlappedPtr->GetSession();
    CrashFunc(session != nullptr);
    session->AsyncAccept(overlappedPtr);
}

void Service::ErrorCode(int32 errorCode)
{
    wchar_t* s = nullptr;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                  nullptr, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  reinterpret_cast<LPWSTR>(&s), 0, nullptr);
    wprintf(L"ErrorCode Service : %d - ErrorMessage : %s", errorCode, s);
}
