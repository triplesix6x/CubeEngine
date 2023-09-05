#ifndef _POOL_ALLOCATOR_H
#define _POOL_ALLOCATOR_H

#include <cstddef>
#include <iostream>
#include <stdint.h>

namespace mem {
	class Pool_allocator final {
		friend std::ostream& operator<<(std::ostream& os, Pool_allocator& pool);
	public:
		Pool_allocator();
		Pool_allocator(const size_t element_sz_bytes, const size_t num_elements, const size_t aligmnent);

		~Pool_allocator();

		void alloc_pool(const size_t element_sz_bytes, const size_t num_elements, const size_t aligmnent);

		void* get_element();
		void  free_element(void* pelement);

		size_t	get_element_size() const;
	private:
		void  realease_pool_mem();

		size_t  m_pool_sz_bytes;    
		size_t  m_element_sz_bytes; 
		size_t  m_alignment;
		void* m_pmemory; 
		void** m_ppfree_memory_list; 
	};

	std::ostream& operator<<(std::ostream& os, Pool_allocator& pool);
}
#endif 