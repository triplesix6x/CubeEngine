#ifndef _RUNTIME_MEMORY_ALLOCATOR_H
#define _RUNTIME_MEMORY_ALLOCATOR_H
#include <cstddef>
namespace mem {
	extern void* allocate(const size_t sz);
	extern void  free(void* pmem, const size_t sz);
}

#endif