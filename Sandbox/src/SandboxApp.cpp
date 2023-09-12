#include <stdint.h>
#include <iostream>
#include <Cube.h>



Cube::Application* Cube::CreateApplication(HINSTANCE hInstance)
{
	return new Application(hInstance);
}

void Cube::ReleaseApplication(void* app)
{	
	return delete app;
}