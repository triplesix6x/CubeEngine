#include "Allocator.h"
#include <iostream>
#include <cstddef>
#include <cstdlib>
#include <cassert>
#include <cstdint>
#include <stdint.h>

namespace mem {


	void* allocate_unaligned(const size_t size_bytes)
	{
		void* pmemory_block = malloc(size_bytes);

		return pmemory_block;
	}

	void free_unaligned(void* pmemory)
	{
		free(pmemory);
	}


	void* allocate_aligned(const size_t size_bytes, const size_t alignment)
	{
		assert(alignment >= 1);
		assert(alignment <= 128);
		assert((alignment & (alignment - 1)) == 0);


		size_t expanded_size_bytes = size_bytes + alignment;


		uintptr_t raw_address = reinterpret_cast<uintptr_t>(allocate_unaligned(expanded_size_bytes));


		size_t mask = alignment - 1;
		uintptr_t misalignment = raw_address & mask;
		ptrdiff_t adjustment = alignment - misalignment;


		uintptr_t aligned_address = raw_address + adjustment;


		assert(adjustment < 256);
		uint8_t* paligned_mem = reinterpret_cast<uint8_t*>(aligned_address);
		paligned_mem[-1] = static_cast<uint8_t>(adjustment);

		return static_cast<void*>(paligned_mem);
	}

	void free_aligned(void* pmemory)
	{
		const uint8_t* paligned_memory = reinterpret_cast<const uint8_t*>(pmemory);
		uintptr_t aligned_address = reinterpret_cast<uintptr_t>(pmemory);
		ptrdiff_t ajustment = static_cast<ptrdiff_t>(paligned_memory[-1]);

		uintptr_t raw_address = aligned_address - ajustment;
		void* praw_memory = reinterpret_cast<void*>(raw_address);

		free_unaligned(praw_memory);
	}

}