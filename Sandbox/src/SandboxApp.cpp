#include <stdint.h>
#include <iostream>
#include <Cube.h>

class Sandbox : public Cube::Application
{
public:
	Sandbox()
	{
		std::cout << sizeof(this) << std::endl;
	}

	~Sandbox()
	{

	}
};

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
	return new Sandbox();
}

void Cube::ReleaseApplication(void* app)
{	
	return delete app;
}