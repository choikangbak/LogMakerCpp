#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/sinks/sink.h"
#include "spdlog/details/log_msg.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/callback_sink1.h"
#include "spdlog/sinks/ostream_sink.h"
#include "spdlog/sinks/qt_sinks.h"
#include "spdlog/common.h"
#include "spdlog/logger.h"
#include "spdlog/details/fmt_helper.h"
//#include <spdlog/details/backtracer.h>
#include <stdarg.h>
#include <sstream>

#include <vector>
#include <pqxx/pqxx>
#include "Log.h"
#include "LogDAO.h"


namespace spdlog
{
	namespace sinks 
	{
//		template<typename Mutex>
		class postgresql_sink : public sink// public base_sink<Mutex>
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
	std::ostringstream m_oss;
	spdlog::level::level_enum m_lvl[5];	// 
	bool m_bSink[5];	// bConsole, bFile, bSql, bQtUi, bCallback
public:
	CleLogger();

	~CleLogger();

	void initCleLogger(const std::string& strDir, QTextEdit* pQEdit, void* func, const std::string& strSqlPasswd);
	void setOnOffSink(bool bConsole, bool bFile, bool bQtUi, bool bCallback, bool bSql)
	{	// 순서 중요
		m_bSink[0] = bConsole;
		m_bSink[1] = bFile;
		m_bSink[2] = bQtUi;
		m_bSink[3] = bCallback;
		m_bSink[4] = bSql;
	};
	void setLevels(spdlog::level::level_enum lCs, spdlog::level::level_enum lF, spdlog::level::level_enum lQt, spdlog::level::level_enum lCb, spdlog::level::level_enum lSql)
	{	// 순서 중요
		m_lvl[0] = lCs;
		m_lvl[1] = lF;
		m_lvl[2] = lQt;
		m_lvl[3] = lCb;
		m_lvl[4] = lSql;
	}

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
