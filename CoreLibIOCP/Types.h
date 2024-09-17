#pragma once
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
using Tuple = std::pair<T, K>;

template <typename T, typename K>
struct TupleHash {
    std::size_t operator()(const Tuple<T, K>& p) const {
        return std::hash<T>()(p.first) ^ std::hash<K>()(p.second); // 비트 연산을 사용하여 두 해시를 결합
    }
};

template <typename T, typename K>
using Map = std::unordered_map<T, K>;

template <typename T1, typename T2, typename K>
using MapTuple = std::unordered_map<Tuple<T1, T2>, K, TupleHash<T1, T2>>; // pair 커스텀 해시 적용

using String = std::string;
using WString = std::wstring;

using Function = std::function<void()>;