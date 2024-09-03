#include "pch.h"
#include "DummySession.h"
#include "DummyPlayerInfo.h"
#include "DummyProto.pb.h"
#include "DummyUtils.h"
#include "GamePacketHandler.h"
#include "SocketConfig.h"

DummySession::DummySession(EndPointUtil& ep) : Session(ep)
{
    static Atomic<int32> id = 0;
    _id = id.fetch_add(1);
    _connOLS.SetType(OverlappedSocket::Type::CONN);
}

DummySession::~DummySession()
{
    std::cout << "~DummySession" << std::endl;
}

void DummySession::SetId(int32 id)
{
    _id = id;
}

void DummySession::OnWait()
{
}

void DummySession::AsyncLoad()
{
}

bool DummySession::AsyncConnect()
{
    if (!SocketConfig::SocketBind(_socket, 0))
        return false;
    
    ZeroMemory(&_serverAddr, sizeof(SOCKADDR_IN));
    _serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, GetEndPointUtil().host.c_str(), &_serverAddr.sin_addr);
    _serverAddr.sin_port = htons(static_cast<u_short>(GetEndPointUtil().port));

    DWORD numOfBytes = 0;
    _connOLS.SetSession(shared_from_this());
    auto result = SocketConfig::lpfnConnectEx(_socket, reinterpret_cast<SOCKADDR*>(&_serverAddr), sizeof(_serverAddr), nullptr, 0, &numOfBytes,
                                reinterpret_cast<LPOVERLAPPED>(&_connOLS));
    if (!result)
    {
        int32 errorCode = ::WSAGetLastError();
        if (errorCode != WSA_IO_PENDING)
        {
            ErrorCode(errorCode);
            _connOLS.SetSession(nullptr);
            return false;
        }
    }
    return true;
}

void DummySession::OnConnect()
{
    Session::OnConnect();
    _connOLS.SetSession(nullptr);
    _playerInfo = std::make_shared<DummyPlayerInfo>();
    _playerInfo->Start();
    std::wstring name = L"Dummy-" + std::to_wstring(_id);
    _playerInfo->SetName(name);
    char* nameToStr = DummyUtils::Utils::WcharToChar(name.c_str());
    std::string* nameStr = new String(nameToStr);

    protocol::DLoad pkt;
    protocol::Unit* unit = new protocol::Unit();
    unit->set_uuid(_id);
    unit->set_code(1);
    unit->set_weaponcode(1);
    unit->set_allocated_name(nameStr);
    unit->set_hp(1);
    unit->set_state(1);
    unit->set_lv(1);
    pkt.set_allocated_unit(unit);
    delete nameToStr;

    SendBufferRef sendBuffer = GamePacketHandler::MakePacketHandler(pkt, protocol::MessageCode::D_LOAD);
    AsyncWrite(sendBuffer);
    
    AsyncRead();
}

