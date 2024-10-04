﻿#include "pch.h"
#include "Session.h"
#include "SendBuffer.h"
#include "SocketConfig.h"

Session::Session(EndPointUtil& ep) : _recvBuffer(4096), _serverAddr(), _recvOLS(), _sendOLS()
{
    _ep.host = ep.host;
    _ep.port = ep.port;
}

Session::~Session()
{
}

void Session::Init()
{
    _socket = SocketConfig::CreateSocket();
    CrashFunc(_socket != INVALID_SOCKET);
    _recvOLS.SetType(OverlappedSocket::Type::READ);
    _sendOLS.SetType(OverlappedSocket::Type::SEND);
    _discOLS.SetType(OverlappedSocket::Type::DISC);
}

void Session::AsyncAccept(OverlappedSocket* overlappedPtr)
{
    DWORD dwBytes;
    bool bRetVal = SocketConfig::lpfnAcceptEx(_serviceRef.lock()->GetServerSocket(), _socket, _recvBuffer.WritePos(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes, reinterpret_cast<LPOVERLAPPED>(overlappedPtr));
    if (!bRetVal)
    {
        int errorCode = WSAGetLastError();
        if (errorCode != WSA_IO_PENDING)
        {
            ErrorCode(errorCode);
            AsyncAccept(overlappedPtr);
        }
    }
}

void Session::OnAccept()
{
    _connected.store(true);
    _serviceRef.lock()->AddSessionRef(shared_from_this());
    AsyncRead();
}

void Session::AsyncRead()
{
    if (!_connected)
    {
        return;
    }
#pragma region AsyncRead
    _recvOLS.SetSession(shared_from_this());

    WSABUF wsaBuf;
    wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer.WritePos());
    wsaBuf.len = 4000;

    DWORD numOfBytes = 0;
    DWORD flags = 0;
#pragma endregion
    if (WSARecv(_socket, &wsaBuf, 1, OUT &numOfBytes, OUT &flags, reinterpret_cast<LPWSAOVERLAPPED>(&_recvOLS), nullptr) == SOCKET_ERROR)
    {
        int errorCode = WSAGetLastError();
        if (errorCode != WSA_IO_PENDING)
        {
            ErrorCode(errorCode);
            _recvOLS.SetSession(nullptr);
            if (errorCode == WSAECONNRESET || errorCode == WSAECONNABORTED)
            {
                AsyncDisconnect();
            }
        }
    }
}

void Session::OnRead(int32 len)
{
    _recvOLS.SetSession(nullptr);
    if (len == 0)
    {
        AsyncDisconnect();
    }
    else
    {
        if (!_recvBuffer.OnWrite(static_cast<int32>(len)))
        {
            AsyncDisconnect();
            return;
        }

        int32 dataSize = _recvBuffer.DataSize();

        BYTE* buffer = _recvBuffer.ReadPos();
        int32 processLen = OnRecv(buffer, dataSize);
        if (!_recvBuffer.OnRead(processLen) || processLen <= 0 || dataSize < processLen)
        {
            AsyncDisconnect();
            return;
        }
        _recvBuffer.Clean();

        AsyncRead();
    }
}

int32 Session::OnRecv(BYTE* buffer, int32 len)
{
    return len;
}

void Session::AsyncWrite(SendBufferRef sendBuffer)
{
    if (!_connected)
    {
        return;
    }
#pragma region AsyncWrite
    if (_sendOLS.GetSession() != nullptr)
    {
        // 현재 WSASend로 iocp queue에 들어가 있고 보내지지 않았을경우 sendBuffer를 대기해둔다.
        AddWriteBuffer(sendBuffer);
        return;
    }

    _sendOLS.SetSession(shared_from_this());
    {
        WriteLockGuard wl(lock);
        for (auto buffer : _waitBuffers)
        {
            _sendBuffers.emplace_back(buffer);
        }
        _sendBuffers.emplace_back(sendBuffer);
        _waitBuffers.clear();
    }

    Vector<WSABUF> wsaBufs;
    for (auto buffer : _sendBuffers)
    {
        WSABUF wsaBuf;
        wsaBuf.buf = reinterpret_cast<char*>(buffer->Buffer());
        wsaBuf.len = buffer->WriteSize();
        wsaBufs.emplace_back(wsaBuf);
    }

    DWORD numOfBytes = 0;
    DWORD flags = 0;
#pragma endregion
    if (WSASend(_socket, wsaBufs.data(), wsaBufs.size(), &numOfBytes, flags, reinterpret_cast<LPWSAOVERLAPPED>(&_sendOLS), nullptr) == SOCKET_ERROR)
    {
        int32 errorCode = WSAGetLastError();
        if (errorCode != WSA_IO_PENDING)
        {
            ErrorCode(errorCode);
            _sendBuffers.clear();
            _sendOLS.SetSession(nullptr);
            if (errorCode == WSAECONNRESET || errorCode == WSAECONNABORTED)
            {
                AsyncDisconnect();
            }
        }
    }
}

void Session::AddWriteBuffer(SendBufferRef sendBuffer)
{
    WriteLockGuard wl(lock);
    _waitBuffers.emplace_back(sendBuffer);
}

void Session::OnWrite(int32 len)
{
    WriteLockGuard wl(lock);
    _sendOLS.SetSession(nullptr);
    _sendBuffers.clear();
}

bool Session::AsyncConnect()
{
    return false;
}

void Session::OnConnect()
{
    _connected.store(true);
}

void Session::AsyncDisconnect()
{
    _discOLS.SetSession(shared_from_this());
    if (_serviceRef.lock() != nullptr)
    {
        _serviceRef.lock()->ReleaseSession(shared_from_this());
    }
    _connected.exchange(false);
    
    auto result = SocketConfig::lpfnDisconnectEx(_socket, reinterpret_cast<LPOVERLAPPED>(&_discOLS), TF_REUSE_SOCKET, 0);
    if (!result)
    {
        int32 errorCode = ::WSAGetLastError();
        if (errorCode != WSA_IO_PENDING)
        {
            ErrorCode(errorCode);
            _discOLS.SetSession(nullptr); // RELEASE_REF
        }
    }
}

void Session::OnDisconnect()
{
    _recvBuffer.Clean();
    _sendBuffers.clear();
    // _connected.exchange(false);

    _recvOLS.SetSession(nullptr);
    _sendOLS.SetSession(nullptr);
    _discOLS.SetSession(nullptr);
    // if (_serviceRef.lock() != nullptr)
    // {
    //     _serviceRef.lock()->ReleaseSession(shared_from_this());
    // }
}

void Session::ErrorCode(int32 errorCode)
{
    wchar_t* s = nullptr;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                  nullptr, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  reinterpret_cast<LPWSTR>(&s), 0, nullptr);
    wprintf(L"ErrorCode Session : %d - ErrorMessage : %s", errorCode, s);
}
