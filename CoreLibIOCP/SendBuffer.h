#pragma once
#include "ReadWriteLock.h"

class SendBuffer
{
public:
    SendBuffer(SendBufferChunkRef sendBufferChunk, uint32 allocSize, BYTE* buffer);
    ~SendBuffer();

    BYTE* Buffer() { return _buffer; }
    BYTE* GetData() { return &_buffer[_writeSize]; }
    uint32 AllocSize() { return _allocSize; }
    uint32 WriteSize() { return _writeSize; }
    void Close(uint32 writeSize);

private:
    BYTE* _buffer = nullptr;
    uint32 _writeSize = 0;
    uint32 _allocSize = 0;
    // 이거 물어둬야지 레퍼런스카운트 유지되서 소멸안됨. 안하면 매니저에서 pop하면 소멸됨
    SendBufferChunkRef _sendBufferChunk;
};

class SendBufferChunk : public std::enable_shared_from_this<SendBufferChunk>
{
    enum
    {
        SEND_BUFFER_CHUNK_SIZE = 40960
    };

public:
    SendBufferChunk();
    ~SendBufferChunk();

    SendBufferRef Open(uint32 size);
    void Close(uint32 size);

    void Reset();

private:
    Vector<BYTE> _chunk;
    bool _use = false;
    uint32 _pos = 0;
};

class SendBufferManager
{
    enum
    {
        INIT_CHUNK_SIZE = 100
    };

public:
    SendBufferManager();
    ~SendBufferManager();

    SendBufferRef Open(uint32 size);
    static void ReleaseBuffer(SendBufferChunk* chunk);

    // 뒤에 const 붙이는 차이 알아내기!!
    SendBufferChunkRef CreateChunk() const;

private:
    Vector<SendBufferChunkRef> _chunks;
    Lock lock;
};

