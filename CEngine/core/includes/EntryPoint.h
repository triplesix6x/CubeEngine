#pragma once

#ifdef CUBE_PLATFORM_WINDOWS

extern Cube::Application* Cube::CreateApplication();
extern void Cube::ReleaseApplication(void* app);

int main(int argc, char** argv)
{
	auto app = Cube::CreateApplication();
	app->run();
	Cube::ReleaseApplication(app);
	return 0;
}

#endif