#include <iostream>
#include <stdint.h>
#include "runtimealloc.h"
#define u32 uint32_t
#define u16 uint16_t



int main()
{
	using namespace mem;
	setlocale(LC_ALL, "");
	static const u32 size = 8;
	u32 *arr = (u32*)allocate(size * sizeof(u32));
	for (int i = 0; i < size; ++i)
	{
		arr[i] = i;
		std::cout << reinterpret_cast<uintptr_t>(&arr[i]) << std::endl;
	}
	free(arr, size * sizeof(int));
	
	std::cout << "----------------------------------" << std::endl;
	u16* brr = (u16*)allocate(size * sizeof(u16));
	for (u16 i = 0; i < size; ++i)
	{
		brr[i] = i;
		std::cout << reinterpret_cast<uintptr_t>(&brr[i]) << std::endl;
	}
	free(brr, size * sizeof(int));

	std::cout << "----------------------------------" << std::endl;
	double* crr = (double*)allocate(size * sizeof(double));
	for (int i = 0; i < size; ++i)
	{
		crr[i] = 0.5;
		std::cout << reinterpret_cast<uintptr_t>(&crr[i]) << std::endl;
	}
	free(crr, size * sizeof(int));
	return 0;
}