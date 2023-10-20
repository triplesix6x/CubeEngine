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
		auto sharedFileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("../log.txt");
		s_CoreLogger = std::make_shared<spdlog::logger>("CUBE_ENGINE", sharedFileSink);
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = std::make_shared<spdlog::logger>("APP", sharedFileSink);
		s_ClientLogger->set_level(spdlog::level::trace);

		s_CoreConLogger = spdlog::stdout_color_mt("CUBE_CON_ENGINE");
		s_CoreConLogger->set_level(spdlog::level::trace);

		s_ClientConLogger = spdlog::stdout_color_mt("APP_CON");
		s_ClientConLogger->set_level(spdlog::level::trace);
		CUBE_CORE_INFO("Initialized Log.");
	}
}