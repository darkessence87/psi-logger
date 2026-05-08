#pragma once

#include <mutex>
#include <queue>

#include "ThreadMap.h"
#include "psi/comm/Subscription.h"

#include "LoggerBase.h"

namespace psi::logger {

class LoggerProxy;

/**
 * @brief Concrete, process-wide `ILoggerSingleton` implementation.
 *
 * One `LoggerSingleton` is created per context label (see `getLoggerInstance()`).
 * It formats log records with a standard header (timestamp, level, file, line,
 * address) and delivers them via `LoggerProxy` to the logger IPC server.
 * While the server is unavailable, messages are buffered in a bounded in-memory
 * queue (up to `MAX_MSG_QUEUE_SZ` entries) and flushed once the server reconnects.
 */
class LoggerSingleton final : public ILoggerSingleton
{
public:
    /// @brief Separator character used between fields in a log record.
    static const char RECORD_SEPARATOR;

    /// @brief Maximum number of messages to buffer when the server is unavailable.
    static const size_t MAX_MSG_QUEUE_SZ;

    /**
     * @brief Construct and connect to the logger IPC service.
     * @param ctx  Short context label (e.g. `"HTTP"`) included in every record.
     */
    LoggerSingleton(const char *);

    /// @brief Disconnect from the IPC service and release resources.
    ~LoggerSingleton() override;

    /// @copydoc ILoggerSingleton::needLog
    bool needLog(LogLevel) const override;

    /// @copydoc ILoggerSingleton::logStream
    std::ostringstream &logStream(LogLevel, const char *, const char *, int, size_t) override;

    /// @copydoc ILoggerSingleton::logStreamShort
    std::ostringstream &logStreamShort() override;

    /// @copydoc ILoggerSingleton::flush
    void flush(bool = false) override;

    /// @copydoc ILoggerSingleton::setLogLevel
    void setLogLevel(LogLevel) override;

private:
    /// @brief React to logger-service availability changes (connect/disconnect).
    void onLoggerServiceStatusUpdate(bool);

    std::string currThreadName();
    void generateHeader(LogLevel);
    void logTo(const std::string &);
    void sendToServer(const std::string &);

private:
    std::mutex m_mtx;
    ThreadMap m_threadMap;
    size_t m_timestampId = 0;
    std::shared_ptr<std::ostringstream> m_logStream;
    const std::string m_ctx;
    LogLevel m_level = LogLevel::LVL_TRACE;

    std::unique_ptr<LoggerProxy> m_loggerProxy;
    uint16_t m_clientId = 0;
    comm::Subscription m_loggerServiceAvailability;
    std::queue<std::string> m_msgQueue;
};

/**
 * @brief Obtain (or lazily create) the `LoggerSingleton` for context @p ctx.
 *
 * The first call with a given @p ctx allocates a singleton on the heap;
 * subsequent calls with the same label return the existing instance.
 * Intentionally never freed — the logger must survive until process exit.
 *
 * @param ctx  Context label for the singleton (e.g. `"HTTP"`).
 * @return     Reference to the `ILoggerSingleton` for @p ctx.
 */
ILoggerSingleton &getLoggerInstance(const char *ctx)
{
    static LoggerSingleton* mainLogger = new LoggerSingleton(ctx);
    return *mainLogger;
}

} // namespace psi::logger
