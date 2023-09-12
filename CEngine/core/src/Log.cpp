#include "../includes/Log.h"
#include <fstream>

namespace Cube
{
	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	void Log::init()
	{
		std::fstream logfile;
		logfile.open("../log.txt", std::ios::out);
		logfile << "";
		logfile.close();
		spdlog::set_pattern("%^[%T] %n: %v%$");
		s_CoreLogger = spdlog::basic_logger_mt("CUBE_ENGINE", "../log.txt");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = spdlog::basic_logger_mt("APP", "../log.txt");
		s_ClientLogger->set_level(spdlog::level::trace);
		CUBE_CORE_WARN("Initialized Log.");
	}
}