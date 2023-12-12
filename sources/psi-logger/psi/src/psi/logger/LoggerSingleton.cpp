#include <ctime>
#include <fstream>

#include "psi/logger/LoggerProxy.h"
#include "psi/logger/LoggerSingleton.h"
#include "psi/thread/ThreadPool.h"

#include "psi/tools/Tools.h"

using namespace std::chrono;

namespace psi::logger {

const char LoggerSingleton::RECORD_SEPARATOR = ','; // '\036';
const size_t LoggerSingleton::MAX_MSG_QUEUE_SZ = 1000;

const char *asString(LogLevel lvl) noexcept
{
    switch (lvl) {
    case LogLevel::LVL_DEBUG:
        return "DEBUG";
    case LogLevel::LVL_ERROR:
        return "ERROR";
    case LogLevel::LVL_WARNING:
        return " WARN";
    case LogLevel::LVL_TRACE:
        return "TRACE";
    case LogLevel::LVL_INFO:
        return " INFO";
    default:
        return "unknown";
    }
}

LoggerSingleton::LoggerSingleton(const char *ctx)
    : m_logStream(std::make_shared<std::ostringstream>())
    , m_ctx(ctx)
{
    m_loggerProxy = std::make_unique<LoggerProxy>();
    m_loggerProxy->connect();
    m_loggerServiceAvailability = m_loggerProxy->isServerAvailableAttribute().subscribeAndGet(
        [this](auto, bool isAvailable) { onLoggerServiceStatusUpdate(isAvailable); });
}

LoggerSingleton::~LoggerSingleton()
{
    m_loggerServiceAvailability.reset();
    m_loggerProxy.reset();
}

void LoggerSingleton::onLoggerServiceStatusUpdate(bool isAvailable)
{
    std::cout << "[LoggerServer] isAvailable: " << std::boolalpha << isAvailable << std::endl;
    if (isAvailable) {
        m_loggerProxy->registerClient(m_ctx, [this](uint16_t clientId) { m_clientId = clientId; });
    } else {
        m_clientId = 0;
    }
}

bool LoggerSingleton::needLog(LogLevel lvl) const
{
    uint8_t currentLvl = static_cast<uint8_t>(m_level);
    uint8_t targetLvl = static_cast<uint8_t>(lvl);

    return currentLvl <= targetLvl;
}

void LoggerSingleton::setLogLevel(LogLevel lvl)
{
    m_level = lvl;
}

std::ostringstream &LoggerSingleton::logStream(LogLevel level, const char *fileName, const char *fnName, int line, size_t thisAddress)
{
    m_mtx.lock();
    const std::string thisAddressStr = thisAddress ? ("0x" + tools::to_hex_string(thisAddress)) : " static";
    generateHeader(level);
    *m_logStream << thisAddressStr << RECORD_SEPARATOR << fileName << RECORD_SEPARATOR << fnName << RECORD_SEPARATOR
                 << std::fixed << std::setprecision(3) << line << RECORD_SEPARATOR;
    return *m_logStream;
}

std::ostringstream &LoggerSingleton::logStreamShort()
{
    m_mtx.lock();
    return *m_logStream;
}

void LoggerSingleton::flush(bool isShort)
{
    if (!isShort) {
        *m_logStream << RECORD_SEPARATOR << std::endl;
    }
    logTo(m_logStream->str());

    m_logStream->str("");
    m_mtx.unlock();
}

void LoggerSingleton::generateHeader(LogLevel lvl)
{
    const std::string logLevel = asString(lvl);
#ifdef _WIN32
    const auto processId = std::to_string(GetCurrentProcessId());
#else
    const auto processId = std::to_string(getpid());
#endif
    *m_logStream << ++m_timestampId << RECORD_SEPARATOR << currTimeLocal() << RECORD_SEPARATOR << logLevel
                 << RECORD_SEPARATOR << processId << RECORD_SEPARATOR << currThreadName() << RECORD_SEPARATOR;
}

std::string LoggerSingleton::currThreadName()
{
    return m_threadMap.currentThreadName();
}

std::string LoggerSingleton::currTimeLocal() const
{
    using namespace std::chrono;

    const auto nextTime = system_clock::now();
    const auto nextTimePrecised = nextTime.time_since_epoch();

    const auto inNanoSec = duration_cast<nanoseconds>(nextTimePrecised);
    const auto inSec = duration_cast<seconds>(inNanoSec);
    const auto nsecs = inNanoSec.count() - inSec.count() * 1'000'000'000;

    struct tm localTime;
    const time_t tt = system_clock::to_time_t(nextTime);
    localtime_s(&localTime, &tt);

    std::ostringstream str;
    str << "" << std::setw(2) << std::setfill('0') << localTime.tm_hour;
    str << ":" << std::setw(2) << std::setfill('0') << localTime.tm_min;
    str << ":" << std::setw(2) << std::setfill('0') << localTime.tm_sec;
    str << "." << std::setw(9) << std::setfill('0') << nsecs;

    return str.str();
}

void LoggerSingleton::logTo(const std::string &str)
{
    std::cout << str;
    sendToServer(str);
}

void LoggerSingleton::sendToServer(const std::string &str)
{
    if (!m_clientId) {
        if (m_msgQueue.size() > MAX_MSG_QUEUE_SZ) {
            m_msgQueue.pop();
        }

        m_msgQueue.emplace(str);
        return;
    }

    while (!m_msgQueue.empty()) {
        auto msg = m_msgQueue.front();
        m_msgQueue.pop();
        m_loggerProxy->log(m_clientId, msg);
    }

    m_loggerProxy->log(m_clientId, str);
}

} // namespace psi::logger