#include <stdint.h>
#include <iostream>
#include <Cube.h>


static PoolAllocator allocator {8};
void* operator new(size_t size)
{
	return allocator.allocate(size);
}
void operator delete(void* ptr, size_t size)
{
	return allocator.deallocate(ptr, size);
}

Cube::Application* Cube::CreateApplication()
{
	return new Application();
}

void Cube::ReleaseApplication(void* app)
{	
	return delete app;
}