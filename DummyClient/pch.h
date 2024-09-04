#pragma once

#ifdef _DEBUG
#pragma comment(lib, "CoreLibIOCP\\Debug\\CoreLibIOCP.lib")
#else
#pragma comment(lib, "CoreLibIOCP\\Release\\CoreLibIOCP.lib")
#endif

#include "CorePch.h"
#include <random>

static std::mt19937_64 rng;
static std::uniform_int_distribution<> dstX{-2500, 2500};
static std::uniform_int_distribution<> dstY{-2000, 2000};
static std::uniform_int_distribution<> rot{0, 360};

using DummyServiceRef = std::shared_ptr<class DummyService>;
using DummySessionRef = std::shared_ptr<class DummySession>;
using DummyPlayerInfoRef = std::shared_ptr<class DummyPlayerInfo>;