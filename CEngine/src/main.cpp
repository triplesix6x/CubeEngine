#include <iostream>
#include <stdint.h>
#include "runtimealloc.h"
#define u32 int32_t



int main()
{
	using namespace mem;
	setlocale(LC_ALL, "");
	static const u32 size = 8;
	u32 *arr = (u32*)allocate(size * sizeof(int));
	for (int i = 0; i < size; ++i)
	{
		arr[i] = i;
	}
	for (int i = 0; i < size; ++i)
	{
		std::cout << reinterpret_cast<uintptr_t>(&arr[i]) << std::endl;
	}
	free(arr, size * sizeof(int));
	return 0;
}