#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/sinks/sink.h"
#include "spdlog/details/log_msg.h"
#include "spdlog/common.h"
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
			LogDAO m_logDao;
			spdlog::logger* m_pLogger = NULL;
//			spdlog::sinks::wincolor_stdout_sink_mt m_pSinkConsole = new spdlog::sinks::wincolor_stdout_sink_mt();
		public:
			void set_pattern(const std::string& pattern) override {};
			void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override {};

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
			}

			void flush() override
			{				
			
			}
		};
	}
}