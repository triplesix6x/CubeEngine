#include "../includes/Log.h"
#include <fstream>

namespace Cube
{
	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	void Log::init()
	{
		std::fstream logfile;
		logfile.open("log.txt", std::ios::out);
		logfile << "";
		logfile.close();
		spdlog::set_pattern("%^[%T] %n: %v%$");
		auto sharedFileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("log.txt");
		s_CoreLogger = std::make_shared<spdlog::logger>("CUBE_ENGINE", sharedFileSink);
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = std::make_shared<spdlog::logger>("APP", sharedFileSink);
		s_ClientLogger->set_level(spdlog::level::trace);

		CUBE_CORE_INFO("Initialized Log.");
	}
}