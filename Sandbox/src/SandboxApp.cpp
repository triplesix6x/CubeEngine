#include <stdint.h>
#include <iostream>
#include <Cube.h>

class Sandbox : public Cube::Application
{
public:
	Sandbox()
	{

	}

	~Sandbox()
	{

	}
};

static PoolAllocator allocator {4096};
void* operator new(size_t size)
{
	return allocator.allocate(size);
}
void operator delete(void* ptr, size_t size)
{
	return allocator.deallocate(ptr, size);
}

int main()
{
	Sandbox* sandbox = new Sandbox();
	sandbox->run();
	delete sandbox;
	return 0;
}