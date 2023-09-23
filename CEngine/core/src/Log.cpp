//Файл работы системы логирования

#include "../includes/Log.h"
#include <fstream>

namespace Cube
{
	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;
	std::shared_ptr<spdlog::logger> Log::s_CoreConLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientConLogger;

	void Log::init()
	{
		std::fstream logfile;
		logfile.open("../log.txt", std::ios::out);
		logfile << "";
		logfile.close();
		spdlog::set_pattern("%^[%T] %n: %v%$");					//Задаем паттерн логирования
		s_CoreLogger = spdlog::basic_logger_mt("CUBE_ENGINE", "../log.txt");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = spdlog::basic_logger_mt("APP", "../log.txt");
		s_ClientLogger->set_level(spdlog::level::trace);

		s_CoreConLogger = spdlog::stdout_color_mt("CUBE_CON_ENGINE");
		s_CoreConLogger->set_level(spdlog::level::trace);

		s_ClientConLogger = spdlog::stdout_color_mt("APP_CON");
		s_ClientConLogger->set_level(spdlog::level::trace);
		CUBE_CORE_WARN("Initialized Log.");
	}
}