#pragma once
#include <memory.h>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Cube {
	class Log
	{
	public:
		static void init();
		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetCoreConLogger() { return s_CoreConLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientConLogger() { return s_ClientConLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
		static std::shared_ptr<spdlog::logger> s_CoreConLogger;
		static std::shared_ptr<spdlog::logger> s_ClientConLogger;

	};
}


//Макросы для логирования в файл
#define CUBE_CORE_ERROR(...)	::Cube::Log::GetCoreLogger()->error(__VA_ARGS__)
#define CUBE_CORE_WARN(...)		::Cube::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define CUBE_CORE_INFO(...)		::Cube::Log::GetCoreLogger()->info(__VA_ARGS__)
#define CUBE_CORE_TRACE(...)	::Cube::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define CUBE_CORE_FATAL(...)	::Cube::Log::GetCoreLogger()->fatal(__VA_ARGS__)

#define CUBE_ERROR(...)		::Cube::Log::GetClientLogger()->error(__VA_ARGS__)
#define CUBE_WARN(...)		::Cube::Log::GetClientLogger()->warn(__VA_ARGS__)
#define CUBE_INFO(...)		::Cube::Log::GetClientLogger()->info(__VA_ARGS__)
#define CUBE_TRACE(...)		::Cube::Log::GetClientLogger()->trace(__VA_ARGS__)
#define CUBE_FATAL(...)		::Cube::Log::GetClientLogger()->fatal(__VA_ARGS__)

#define CUBE_CORE_CON_ERROR(...)	::Cube::Log::GetCoreConLogger()->error(__VA_ARGS__)
#define CUBE_CORE_CON_WARN(...)		::Cube::Log::GetCoreConLogger()->warn(__VA_ARGS__)
#define CUBE_CORE_CON_INFO(...)		::Cube::Log::GetCoreConLogger()->info(__VA_ARGS__)
#define CUBE_CORE_CON_TRACE(...)	::Cube::Log::GetCoreConLogger()->trace(__VA_ARGS__)
#define CUBE_CORE_CON_FATAL(...)	::Cube::Log::GetCoreConLogger()->fatal(__VA_ARGS__)

#define CUBE_CON_ERROR(...)		::Cube::Log::GetClientConLogger()->error(__VA_ARGS__)
#define CUBE_CON_WARN(...)		::Cube::Log::GetClientConLogger()->warn(__VA_ARGS__)
#define CUBE_CON_INFO(...)		::Cube::Log::GetClientConLogger()->info(__VA_ARGS__)
#define CUBE_CON_TRACE(...)		::Cube::Log::GetClientConLogger()->trace(__VA_ARGS__)
#define CUBE_CON_FATAL(...)		::Cube::Log::GetClientConLogger()->fatal(__VA_ARGS__)
