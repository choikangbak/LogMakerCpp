#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/sinks/sink.h"
#include "spdlog/details/log_msg.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/common.h"
#include "spdlog/logger.h"
#include "spdlog/details/fmt_helper.h"

#include <spdlog/details/backtracer.h>

#include <vector>
#include <pqxx/pqxx>
#include "Log.h"
#include "LogDAO.h"


namespace spdlog
{
	namespace sinks 
	{
		class postgresql_sink : public sink
		{
		private:
			CRITICAL_SECTION cs;
			LogDAO m_logDao;
		public:
			void set_pattern(const std::string& pattern) override {};
			void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override {};

			explicit postgresql_sink(const std::string& strPasswd);
			~postgresql_sink();

			void log(const spdlog::details::log_msg& msg) override;

			void flush() override {};
		};
	}
}


class CleLogger
{
private:
	spdlog::logger* m_pLogger;

public:
	CleLogger();

	~CleLogger();

	void initCleLogger(const std::string& strSqlPasswd, spdlog::level::level_enum lvl);

	template<typename... Args>	void trace(const char* fmt, const Args &...args) { m_pLogger->trace(fmt, args); }
	template<typename... Args>	void debug(const char* fmt, const Args &...args) { m_pLogger->debug(fmt, args); }
	template<typename... Args>	void info(const char* fmt, const Args &...args) { m_pLogger->info(fmt, args); }
	template<typename... Args>	void warn(const char* fmt, const Args &...args) { m_pLogger->warn(fmt, args); }
	template<typename... Args>	void error(const char* fmt, const Args &...args) { m_pLogger->error(fmt, args); }
	template<typename... Args>	void critical(const char* fmt, const Args &...args) { m_pLogger->critical(fmt, args); }

	template<typename T > void 	trace(const T& msg) { m_pLogger->trace(msg); }
	template<typename T > void 	debug(const T& msg) { m_pLogger->debug(msg); }
	template<typename T > void 	info(const T& msg) { m_pLogger->info(msg); }
	template<typename T > void 	warn(const T& msg) { m_pLogger->warn(msg); }
	template<typename T > void 	error(const T& msg) { m_pLogger->error(msg); }
	template<typename T > void 	critical(const T& msg) { m_pLogger->critical(msg); }
	
};
