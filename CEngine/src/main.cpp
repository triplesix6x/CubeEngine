#include <iostream>
#include <stdint.h>
#include "PoolAllocator.h"
#define u32 uint32_t
#define u16 uint16_t

struct vecc
{

    u32 x;
    u32 y;
    u32 z;

    static PoolAllocator allocator;

    static void* operator new(size_t size) {
        return allocator.allocate(size);
    }

    static void operator delete(void* ptr, size_t size) {
        return allocator.deallocate(ptr, size);
    }
};
PoolAllocator vecc::allocator{ 12 };



int main()
{
    vecc *myvec = new vecc{1, 2, 3};
    delete myvec;
	return 0;
}