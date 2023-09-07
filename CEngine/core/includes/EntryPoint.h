#pragma once

#ifdef CUBE_PLATFORM_WINDOWS

extern Cube::Application* Cube::CreateApplication();
extern void Cube::ReleaseApplication(void* app);

int main(int argc, char** argv)
{
	Cube::Log::init();
	CUBE_CORE_WARN("Initialized Log!");
	CUBE_INFO("Hello!");
	auto app = Cube::CreateApplication();
	app->run();
	Cube::ReleaseApplication(app);
	return 0;
}

#endif