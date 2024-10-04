#pragma once

class Lock
{
    enum
    {
        EMPTY = 0x0000000,
        READ  = 0x000FFFF,
        WRITE = 0xFFF0000,
    };
public:
    Lock();
    void ReadLock();
    void ReadUnLock();
    void WriteLock();
    void WriteUnLock();
private:
    const int MAX_SPIN_COUNT = 5000;
    volatile long  sharedConunt = 0;
};

class ReadLockGuard
{
public:
    ReadLockGuard(Lock& lock) : _lock(lock)
    {
        _lock.ReadLock();
    }

    ~ReadLockGuard()
    {
        _lock.ReadUnLock();
    }

private:
    Lock &_lock;
};

class WriteLockGuard
{
public:
    WriteLockGuard(Lock& lock) : _lock(lock)
    {
        _lock.WriteLock();
    }

    ~WriteLockGuard()
    {
        _lock.WriteUnLock();
    }

private:
    Lock &_lock;
};