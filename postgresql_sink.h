#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/sinks/sink.h"
#include "spdlog/details/log_msg.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/common.h"
#include <vector>
#include <pqxx/pqxx>
#include <time.h>
#include "Log.h"
#include "LogDAO.h"


namespace spdlog
{
	namespace sinks 
	{
		class postgresql_sink : public sink
		{
		private:
			LogDAO m_logDao;
			spdlog::logger* m_pLogger = NULL;
			
		public:
			void set_pattern(const std::string& pattern) override {};
			void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override {};

			void initSpdLog(const std::string& strPasswd)
			{
				auto console_sink = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
				console_sink->set_level(spdlog::level::info);
				console_sink->set_pattern("[multi_sink_example] [%^%l%$] %v");

				time_t timer = time(NULL);
				struct tm now;
				localtime_s(&now, &timer);
				char logFile[27];
				strftime(logFile, sizeof(logFile), "log/%Y%m%d-HH:mm:ss.log", &now);

				auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFile, true);
				file_sink->set_level(spdlog::level::warn);

				auto postgresql_sink = std::make_shared<spdlog::sinks::postgresql_sink>(strPasswd);
				postgresql_sink->set_level(spdlog::level::trace);

				m_pLogger = new spdlog::logger("multi_sink", { console_sink, file_sink, postgresql_sink });
				m_pLogger->set_level(spdlog::level::trace);
			}

			explicit postgresql_sink(const std::string& strPasswd)
			{	
				m_logDao.initLogDao(strPasswd);
			}			

			void log(const spdlog::details::log_msg& msg) override
			{
				Log log;

				// timestamp
				auto time = std::chrono::system_clock::to_time_t(msg.time);
				char str[26];
				ctime_s(str, sizeof(str), &time);				
				log.setTimestamp(str);

				// level
				log.setLevel(level::to_short_c_str(msg.level));

				// message
				std::string m = "";
				for (size_t i = 0; i < msg.payload.size(); i++)
				{
					m += msg.payload.data()[i];
				}
				log.setMessage(m);

				m_logDao.insertLog(log);
			}			

			~postgresql_sink()
			{
				m_logDao.closeConnection();
				if (m_pLogger)
					delete m_pLogger;
			}

			void flush() override
			{				
			
			}
		};
	}
}