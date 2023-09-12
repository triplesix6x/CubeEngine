#include "../includes/PoolAllocator.h"
#include <iostream>
#include <stdint.h>
#include <memory>
#include <assert.h>


Chunk* PoolAllocator::allocatePool(size_t chunkSize) 
{
    assert((chunkSize <= mPoolSize) && "Выделаяемый объект больше общего пула");
    mChunksPerPool = mPoolSize / chunkSize;
    Chunk* poolBegin = reinterpret_cast<Chunk*>(malloc(mPoolSize));
    Chunk* chunk = poolBegin;

    for (int i = 0; i < mChunksPerPool - 1; ++i) 
    {
        chunk->next = reinterpret_cast<Chunk*>(reinterpret_cast<char*>(chunk) + chunkSize);
        chunk = chunk->next;
    }

        chunk->next = nullptr;
        return poolBegin;
 }

void* PoolAllocator::allocate(size_t size)
    {
        if (mAlloc == nullptr)
        {
            mAlloc = allocatePool(size);
        }

        Chunk* freeChunk = mAlloc;

        mAlloc = mAlloc->next;
        return freeChunk;
    }

    void PoolAllocator::deallocate(void* chunk, size_t size)
    {
        reinterpret_cast<Chunk*>(chunk)->next = mAlloc;
        mAlloc = reinterpret_cast<Chunk*>(chunk);
    }

    void PoolAllocator::realeasePool()
    {
        free(reinterpret_cast<void*>(mAlloc));
    }

    PoolAllocator::~PoolAllocator()
    {
        realeasePool();
    }
