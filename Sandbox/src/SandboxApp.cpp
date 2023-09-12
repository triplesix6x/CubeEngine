#include <stdint.h>
#include <iostream>
#include <Cube.h>



Cube::Application* Cube::CreateApplication()
{
	return new Application();
}

void Cube::ReleaseApplication(void* app)
{	
	return delete app;
}