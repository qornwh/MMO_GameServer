#pragma once
#include "Types.h"
#include "CoreGlobal.h"
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <assert.h>
#include "nlohmann/json.hpp"
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "CRYPT32.lib")
#pragma comment(lib, "Normaliz.lib")

void CrashFunc(bool result);
