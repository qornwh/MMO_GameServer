﻿#pragma once
#include <memory>
#include <atomic>
#include <queue>
#include <set>
#include <functional>

using SessionRef = std::shared_ptr<class Session>;
using SendBufferRef = std::shared_ptr<class SendBuffer>;
using SendBufferChunkRef = std::shared_ptr<class SendBufferChunk>;
using SendBufferManagerRef = std::shared_ptr<class SendBufferManager>;
using ServiceRef = std::shared_ptr<class Service>;

using BYTE = uint8_t;
using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;
using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

template <typename T>
using Atomic = std::atomic<T>;

template <typename T>
using Vector = std::vector<T>;

template <typename T>
using Queue = std::queue<T>;

template <typename T>
using PriorityQueueDes = std::priority_queue<T, Vector<T>, std::greater<T>>;

template <typename T>
using PriorityQueue = std::priority_queue<T>;

template <typename T>
using Set = std::set<T>;

template <typename T, typename K>
using Map = std::unordered_map<T, K>;

template <typename T, typename K>
using Tuple = std::pair<T, K>;

using String = std::string;

using Function = std::function<void()>;