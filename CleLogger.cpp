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

void CleLogger::initCleLogger(const std::string& strSqlPasswd, spdlog::level::level_enum lvl, void* func)
{
	auto sinkConsole = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
	//sinkConsole->set_level(lvl);
	sinkConsole->set_pattern("[%^%l%$] %v");

	time_t timer = time(NULL);
	struct tm now;
	localtime_s(&now, &timer);
	char logFile[128]; //[27];
	strftime(logFile, sizeof(logFile), "logs/%Y%m%d-%H%M%S.log", &now);

	auto sinkFile = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logFile, 1024*1024*1024, 30);	// 1기가씩 최대 ?개
	//sinkFile->set_level(lvl);

	auto sinkGui = std::make_shared<spdlog::sinks::callback_sink_mt>((CALLBACK_FUNC)func);

	//sinkGui->set_level(lvl);

	auto sinkOss = std::make_shared<spdlog::sinks::ostream_sink_mt>(m_oss);

	spdlog::flush_every(std::chrono::seconds(1));

	try {
		auto sinkPostgre = std::make_shared<spdlog::sinks::postgresql_sink>("cle");
		sinkPostgre->set_level(lvl);

		m_pLogger = new spdlog::logger("CleSink", { sinkConsole, sinkFile, sinkGui, sinkPostgre });
	}
	catch (...) {
		std::cout << "initSpdLog postgresql_sink Error. logs can not to reach DB" << std::endl;
		m_pLogger = new spdlog::logger("CleSink", { sinkConsole, sinkFile, sinkGui });
	}
	//m_pLogger->set_level(lvl);
}
