#include "pch.h"
#include "ReadWriteLock.h"
#include <thread>

Lock::Lock()
{
}

void Lock::ReadLock()
{
    while (true)
    {
        for (int i = 0; i < MAX_SPIN_COUNT; i++)
        {
            auto currentCount = sharedConunt & READ;
            auto orignCount = InterlockedCompareExchange(&sharedConunt, currentCount + 1, currentCount);
            if (currentCount == orignCount)
            {
                return;
            }
        }
        std::this_thread::yield();
    }
}

void Lock::ReadUnLock()
{
    auto currentCount = sharedConunt;
    if (currentCount == 0) 
        CrashFunc(false);
    InterlockedDecrement(&sharedConunt);
}

void Lock::WriteLock()
{
    while (true)
    {
        for (int i = 0; i < MAX_SPIN_COUNT; i++)
        {
            auto exchangeCount = WRITE;
            long compareCount = EMPTY;
            auto orignCount = InterlockedCompareExchange(&sharedConunt, exchangeCount, compareCount);
            if (compareCount == orignCount)
            {
                return;
            }
        }
        std::this_thread::yield();
    }
}

void Lock::WriteUnLock()
{
    auto exchangeCount = EMPTY;
    long compareCount = WRITE;
    auto orignCount = InterlockedCompareExchange(&sharedConunt, exchangeCount, compareCount);
    if (compareCount != orignCount)
    {
        CrashFunc(false);
    }
}
