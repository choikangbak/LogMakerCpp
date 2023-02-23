#include "CleLogger.h"


void spdlog::sinks::postgresql_sink::log(const spdlog::details::log_msg& msg)
{
	::EnterCriticalSection(&cs);
	Log log;

	auto time = std::chrono::system_clock::to_time_t(msg.time);	// timestamp
	char chTime[26];
	ctime_s(chTime, sizeof(chTime), &time);
	log.setTimestamp(chTime);

	log.setLevel(level::to_short_c_str(msg.level));	// level

	std::string strMsg = "";
	for (int i = 0; i < msg.payload.size(); i++)	// message
		strMsg += msg.payload.data()[i];

	log.setMessage(strMsg);

	m_logDao.insertLog(log);
	::LeaveCriticalSection(&cs);
}

spdlog::sinks::postgresql_sink::postgresql_sink(const std::string& strPasswd)
{
	m_logDao.initLogDao(strPasswd);
	::InitializeCriticalSection(&cs);
}

spdlog::sinks::postgresql_sink::~postgresql_sink()
{
	m_logDao.closeConnection();
	::DeleteCriticalSection(&cs);
}

CleLogger::CleLogger()
{
	m_pLogger = NULL;
	memset(m_bSink, false, sizeof(m_bSink));
	memset(m_lvl, spdlog::level::trace, sizeof(m_lvl));
}

CleLogger::~CleLogger()
{
	if (m_pLogger)		delete m_pLogger;
}

void CleLogger::initCleLogger(const std::string& strDir, QTextEdit* pQEdit, void* func, const std::string& strSqlPasswd)
{
	std::vector<spdlog::sink_ptr> sinks;
	if (m_bSink[0]) {
		sinks.push_back(std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>());
		sinks[0]->set_pattern("[%^%l%$] %v");
		sinks[0]->set_level(m_lvl[0]);
	}
	if (m_bSink[1]) {
		time_t timer = time(NULL);
		struct tm now;	localtime_s(&now, &timer);

		char logFile[128]; //[27];
		std::string strFormat = strDir + "/%Y%m%d-%H%M%S.log";
		strftime(logFile, sizeof(logFile), strFormat.c_str(), &now);

		sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logFile, 1024 * 1024 * 1024, 30));	// 1기가씩 최대 ?개
		sinks.back()->set_level(m_lvl[1]);
	}
	if (m_bSink[2] && pQEdit) {
		sinks.push_back(std::make_shared<spdlog::sinks::qt_sink_mt>(pQEdit, "append"));
		sinks.back()->set_level(m_lvl[2]);
	}
	if (m_bSink[3] && func) {
		sinks.push_back(std::make_shared < spdlog::sinks::callback_sink_mt>((CALLBACK_FUNC)func));
		sinks.back()->set_level(m_lvl[3]);
	}
	if (m_bSink[4]) {
		try {
			auto sinkPostgre = std::make_shared<spdlog::sinks::postgresql_sink>(strSqlPasswd);
			sinks.push_back(sinkPostgre);
			sinks.back()->set_level(m_lvl[4]);
		}
		catch (...) {
			std::cout << "initSpdLog postgresql_sink Error. logs can not to reach DB" << std::endl;
		}
	}
//	auto sinkOss = std::make_shared<spdlog::sinks::ostream_sink_mt>(m_oss);
	m_pLogger = new spdlog::logger("CleSink", sinks.begin(), sinks.end());

	m_pLogger->set_level(spdlog::level::trace);

	spdlog::flush_every(std::chrono::seconds(1));	// 확인해야함
}
