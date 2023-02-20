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
}
CleLogger::~CleLogger()
{
	if (m_pLogger)		delete m_pLogger;
}

void CleLogger::initCleLogger(const std::string& strSqlPasswd, spdlog::level::level_enum lvl)
{
	auto console_sink = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
	console_sink->set_level(lvl);
	console_sink->set_pattern("[%^%l%$] %v");

	time_t timer = time(NULL);
	struct tm now;
	localtime_s(&now, &timer);
	char logFile[128]; //[27];
	strftime(logFile, sizeof(logFile), "logs/%Y%m%d-%H%M%S.log", &now);

	auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logFile, 1024*1024*1024, 5);	// 1기가씩 최대 5개
	file_sink->set_level(lvl);
	spdlog::flush_every(std::chrono::seconds(1));

	try {
		auto postgresql_sink = std::make_shared<spdlog::sinks::postgresql_sink>("cle");
		postgresql_sink->set_level(lvl);

		m_pLogger = new spdlog::logger("CleSink", { console_sink, file_sink, postgresql_sink });
	}
	catch (...) {
		std::cout << "initSpdLog postgresql_sink Error. logs can not to reach DB" << std::endl;
		m_pLogger = new spdlog::logger("CleSink", { console_sink, file_sink });
	}
	m_pLogger->set_level(lvl);
}
