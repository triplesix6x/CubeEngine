#include "PoolAllocator.h"
#include "Allocator.h"

#include <cassert>
#include <cstdint>
#include <cstddef>
#include <iostream>
#include <stdint.h>


namespace mem {

	Pool_allocator::Pool_allocator() :
		m_pool_sz_bytes(0), m_element_sz_bytes(0), m_alignment(0), m_pmemory(nullptr), m_ppfree_memory_list(nullptr) {}


	Pool_allocator::Pool_allocator(const size_t element_sz_bytes, const size_t num_elements, const size_t aligmnent)
	{
		m_pmemory = m_ppfree_memory_list = nullptr;
		alloc_pool(element_sz_bytes, num_elements, aligmnent);
	}


	void Pool_allocator::alloc_pool(const size_t element_sz_bytes, const size_t num_elements, const size_t aligmnent)
	{
		assert(m_pmemory == nullptr);

		m_element_sz_bytes = element_sz_bytes;
		m_alignment = aligmnent;

		assert(m_element_sz_bytes >= sizeof(void*));

		assert(m_element_sz_bytes % m_alignment == 0);

		assert((m_alignment & (m_alignment - 1)) == 0);

		m_pool_sz_bytes = (m_element_sz_bytes * num_elements) + aligmnent;


		m_pmemory = allocate_aligned(m_element_sz_bytes * num_elements, m_alignment);
		if (m_pmemory == nullptr) { 
			std::cerr << "Ошибка  " << __FUNCTION__ << ": Невозможна аллокация " << std::endl;
		}
		else {
			m_ppfree_memory_list = static_cast<void**>(m_pmemory);

			uintptr_t end_address = reinterpret_cast<uintptr_t>(m_ppfree_memory_list) + (m_element_sz_bytes * num_elements);

			for (size_t element_cnt = 0; element_cnt < num_elements; ++element_cnt) {

				uintptr_t curr_address = reinterpret_cast<uintptr_t>(m_ppfree_memory_list) + element_cnt * m_element_sz_bytes;
				uintptr_t next_address = curr_address + m_element_sz_bytes;

				void** curr_memory = reinterpret_cast<void**>(curr_address);
				if (next_address >= end_address) { 
					*curr_memory = nullptr;
				}
				else {
					*curr_memory = reinterpret_cast<void*>(next_address);
				}
			}
		}
	}

	void* Pool_allocator::get_element()
	{
		if (m_pmemory == nullptr) {
			std::cerr << "Ошибка " << __FUNCTION__ << ": Не было аллокации для этого блока " << std::endl;
			return nullptr;
		}

		if (m_ppfree_memory_list != nullptr) {
			void* pblock = reinterpret_cast<void*>(m_ppfree_memory_list);
			m_ppfree_memory_list = static_cast<void**>(*m_ppfree_memory_list);

			return pblock;
		}
		else { 
			std::cerr << "Ошибка " << __FUNCTION__ << ": Больше нет блоков памяти" << std::endl;
			return nullptr;
		}
	}

	void Pool_allocator::free_element(void* pblock)
	{
		if (pblock == nullptr) {
			return;
		}

		if (m_pmemory == nullptr) {
			std::cerr << "Ошибка " << __FUNCTION__ << ": Не было аллокации для этого блока " << std::endl;
			return;
		}

		if (m_ppfree_memory_list == nullptr) { 
			m_ppfree_memory_list = reinterpret_cast<void**>(pblock);
			*m_ppfree_memory_list = nullptr;
		}
		else {
			void** ppreturned_block = m_ppfree_memory_list;
			m_ppfree_memory_list = reinterpret_cast<void**>(pblock);
			*m_ppfree_memory_list = reinterpret_cast<void*>(ppreturned_block);
		}

	}


	void Pool_allocator::realease_pool_mem()
	{
		free_aligned(m_pmemory);
		m_pmemory = m_ppfree_memory_list = nullptr;
	}

	Pool_allocator::~Pool_allocator()
	{
		realease_pool_mem();
	}

	size_t Pool_allocator::get_element_size() const
	{
		return m_element_sz_bytes;
	}

	std::ostream& operator<<(std::ostream& os, Pool_allocator& pool)
	{
		if (pool.m_pmemory == nullptr) {
			os << "Ошибка " << __FUNCTION__ << ": Не было аллокации для этого блока" << std::endl;
			return os;
		}

		size_t num_elements = (pool.m_pool_sz_bytes - pool.m_alignment) / pool.m_element_sz_bytes;
		os << "Информация о памяти пула: полный размер " << pool.m_pool_sz_bytes << " байт| " << std::endl
			<< pool.m_element_sz_bytes << " байт размер блока| " << std::endl << num_elements << " элементов| " << std::endl << pool.m_alignment << " байт требование к выравниванию|" << std::endl;
		os << "Вывод списка свободной памяти пула:     "
			<< "адресс памяти | Следующий указатель" << std::endl;
		void** pplist = pool.m_ppfree_memory_list;
		for (size_t element_cnt = 0; element_cnt < num_elements; ++element_cnt) {
			if (pplist == nullptr) {
				break;
			}

			uintptr_t curr_address = reinterpret_cast<uintptr_t>(pplist);
			uintptr_t next_address = reinterpret_cast<uintptr_t>(*pplist);

			pplist = static_cast<void**>(*pplist);
			os << "\t 0x" << std::hex << curr_address << " | 0x" << next_address << std::dec << std::endl;
		}

		return os;
	}
}