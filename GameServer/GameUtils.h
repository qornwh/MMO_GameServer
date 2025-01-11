#pragma once
#include "pch.h"
#include <WinNls.h>
#include <Rpc.h>
#include <nlohmann/json.hpp>
#pragma comment(lib, "Rpcrt4.lib")

#define UUIDSIZE 37

namespace GameUtils
{
    class JsonParser
    {
    public:
        static nlohmann::json GetStrParser(String& jsonStr)
        {
            return nlohmann::json::parse(jsonStr);
        }

        static nlohmann::json Parser(String key, const nlohmann::json& json)
        {
            // 그냥 바로 크래시 낸다 !!!
            CrashFunc(!json.at(key).is_null());

            return json.at(key);
        }
    };

    class TickCounter
    {
    public:
        TickCounter(int32 value) : _value(value)
        {
        }

        ~TickCounter()
        {
        }

        int32 Add(int32 value = 1)
        {
            _tick = (_tick + value) % _value;
            return _tick;
        }

        int32 GetTick()
        {
            return _tick;
        }

        int32 GetTickValue()
        {
            return _value;
        }

        void ResetTic(int32 value = -1)
        {
            _tick = value;
        }

    private:
        int32 _value;
        int32 _tick = 0;
    };

    class Utils
    {
    public:
        static wchar_t* CharToWchar(const char* cPtr, WCHAR* wPtr)
        {
            int32 isize = static_cast<int32>(strlen(cPtr));
            int32 nLength = MultiByteToWideChar(CP_UTF8, 0, cPtr, isize, NULL, NULL);
            int32 nLen = sizeof(wchar_t) * (nLength + 1);
            memset(wPtr, 0, nLen);
            MultiByteToWideChar(CP_UTF8, 0, cPtr, isize, wPtr, nLength);
            return wPtr;
        }

        static char* WcharToChar(const wchar_t* wPtr, char* cPtr)
        {
            int32 nLength = WideCharToMultiByte(CP_UTF8, 0, wPtr, -1, NULL, 0, NULL, NULL);
            memset(cPtr, 0, nLength + 1);
            WideCharToMultiByte(CP_UTF8, 0, wPtr, -1, cPtr, nLength, NULL, NULL);
            return cPtr;
        }

        static bool GenUUID(UUID* uuid)
        {
            RPC_STATUS status = UuidCreate(uuid);
            if (status == RPC_S_OK)
            {
                return true;
            }
            return false;
        }

        static void UUIDToArray(UUID* uuid, void* cPtr)
        {
            sprintf_s(static_cast<char*>(cPtr), UUIDSIZE, "%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                uuid->Data1, uuid->Data2, uuid->Data3, 
                uuid->Data4[0], uuid->Data4[1], uuid->Data4[2],
                uuid->Data4[3], uuid->Data4[4], uuid->Data4[5], 
                uuid->Data4[6], uuid->Data4[7]);
        }
    };
}
