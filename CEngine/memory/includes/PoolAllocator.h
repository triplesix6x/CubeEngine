#pragma once
#include <stdint.h>
#include <memory>



struct Chunk
    {
        Chunk* next;
    };

class PoolAllocator
    {
    public:
        PoolAllocator(size_t poolSize) : mPoolSize(poolSize) {}

        void* allocate(size_t size);
        void deallocate(void* ptr, size_t size);
        void realeasePool();
        ~PoolAllocator();

    private:

        size_t mPoolSize;
        size_t mChunksPerPool;
        Chunk* mAlloc = nullptr;

        Chunk* allocatePool(size_t chunkSize);
    };
