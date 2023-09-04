#ifndef _MEMORY_ALLOCATOR_H
#define _MEMORY_ALLOCATOR_H
#include <cstddef>
#include <stdint.h>

namespace mem {


	void* allocate_unaligned(const size_t size_bytes);
	void free_unaligned(void* pmemory);

	void* allocate_aligned(const  size_t size_bytes, const size_t alignment);
	void  free_aligned(void* pmemory);
}
#endif