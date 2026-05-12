#pragma once

#include <cstdint>
#include <sstream>

namespace psi::logger {

/**
 * @brief Extract the filename component from a full file path at compile time.
 *
 * Scans @p path for the last @c '/' or @c '\\' separator and returns a
 * pointer to the character immediately following it, so only the bare
 * filename (without any directory prefix) appears in log records.
 *
 * @param path  Null-terminated file path (typically `__FILE__`).
 * @return Pointer into @p path at the start of the filename.
 */
consteval const char *filename(const char *path)
{
    const char *file = path;
    while (*path) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
        if (*path == '/' || *path == '\\') {
            file = path + 1;
        }
        ++path;
#pragma clang diagnostic pop
    }
    return file;
}

/**
 * @brief Severity levels for log messages, ordered from least to most severe.
 */
enum class LogLevel : uint8_t
{
    LVL_TRACE = 1, ///< Fine-grained diagnostic trace.
    LVL_DEBUG,     ///< Debug information useful during development.
    LVL_INFO,      ///< Informational messages about normal operation.
    LVL_WARNING,   ///< Potentially harmful situations that are still recoverable.
    LVL_ERROR      ///< Error events that may still allow the application to continue.
};

/// @brief Return a short human-readable string for @p lvl (e.g. `"DEBUG"`, `" WARN"`).
const char *asString(LogLevel) noexcept;

/**
 * @brief Abstract interface for the per-context logger singleton.
 *
 * `LoggerSingleton` implements this interface.  User code obtains a reference
 * via `getLoggerInstance()` and interacts with it exclusively through the
 * logging macros defined in Logger.h / LoggerBase.h.
 */
class ILoggerSingleton
{
public:
    virtual ~ILoggerSingleton() = default;

    /**
     * @brief Return @c true if a message at @p level should be recorded.
     *
     * Used by the logging macros to skip message construction when the
     * current log level is higher than the requested one.
     */
    virtual bool needLog(LogLevel) const = 0;

    /**
     * @brief Prepare and return the output stream for a structured log entry.
     *
     * Writes the standard header fields (timestamp, level, file, function,
     * line, address) into the internal stream and returns a reference to it
     * so the caller can stream the message body.
     *
     * @param level     Severity of the message.
     * @param file      Source file name (basename only).
     * @param function  Name of the enclosing function.
     * @param line      Source line number.
     * @param address   `this` pointer cast to `size_t`, or 0 for static context.
     * @return Reference to the internal `std::ostringstream`.
     */
    virtual std::ostringstream &logStream(LogLevel, const char *, const char *, int, size_t) = 0;

    /**
     * @brief Return the stream for a short (header-less) log entry.
     *
     * Used by `LOG_MSG_SHORT` when only the raw message body is needed.
     */
    virtual std::ostringstream &logStreamShort() = 0;

    /**
     * @brief Flush the pending log entry to the transport.
     * @param isShort Pass @c true when flushing a short (header-less) entry.
     */
    virtual void flush(bool /*isShort*/ = false) = 0;

    /**
     * @brief Set the minimum severity level; messages below this level are discarded.
     * @param level The new minimum log level.
     */
    virtual void setLogLevel(LogLevel) = 0;
};

/**
 * @brief Obtain (or create) the `ILoggerSingleton` for context label @p ctx.
 *
 * On the first call with a given @p ctx string a new `LoggerSingleton` is
 * constructed and stored as a process-wide singleton for that context.
 * Subsequent calls with the same label return the existing instance.
 *
 * @param ctx  Short context tag shown in every log record (e.g. `"HTTP"`).
 * @return     Reference to the singleton for @p ctx.
 */
ILoggerSingleton &getLoggerInstance(const char *);

} // namespace psi::logger

/**
 * @brief Override the context label used by the logging macros.
 *
 * Define this macro **before** including `Logger.h` / `LoggerBase.h` to tag
 * all log records from the current translation unit with a custom label.
 * Defaults to `"LOGG"` if not defined.
 */
#ifndef LOG_CTX
#define LOG_CTX "LOGG"
#endif

/**
 * @brief Core logging macro for instance methods (captures `this` address).
 *
 * Checks `needLog(level)` first to avoid unnecessary string construction.
 * Streams `x` into the log entry and flushes it to the transport.
 * Not intended for direct use — prefer the typed aliases in Logger.h.
 */
#define LOG_MSG(x, level)                                                                                              \
    do {                                                                                                               \
        auto &logger = psi::logger::getLoggerInstance(LOG_CTX);                                                        \
        if (!logger.needLog(level)) {                                                                                  \
            break;                                                                                                     \
        }                                                                                                              \
        const size_t _thisAddress_ = reinterpret_cast<size_t>(this);                                                   \
        auto &logStream =                                                                                              \
            logger.logStream(level, psi::logger::filename(__FILE__), __FUNCTION__, __LINE__, _thisAddress_);           \
        logStream << x;                                                                                                \
        logger.flush();                                                                                                \
    } while (0)

/**
 * @brief Core logging macro for static / free functions (address reported as 0).
 *
 * Identical to `LOG_MSG` but passes address 0 instead of `this`.
 * Not intended for direct use — prefer the typed aliases in Logger.h.
 */
#define LOG_MSG_STATIC(x, level)                                                                                       \
    do {                                                                                                               \
        auto &logger = psi::logger::getLoggerInstance(LOG_CTX);                                                        \
        if (!logger.needLog(level)) {                                                                                  \
            break;                                                                                                     \
        }                                                                                                              \
        auto &logStream = logger.logStream(level, psi::logger::filename(__FILE__), __FUNCTION__, __LINE__, 0);         \
        logStream << x;                                                                                                \
        logger.flush();                                                                                                \
    } while (0)

/**
 * @brief Emit a short (header-less) log entry.
 *
 * Writes only the raw message body — no timestamp, level, or file information.
 * Useful for continuing a multi-part log line started elsewhere.
 */
#define LOG_MSG_SHORT(x)                                                                                               \
    do {                                                                                                               \
        auto &logger = psi::logger::getLoggerInstance(LOG_CTX);                                                        \
        auto &logStream = logger.logStreamShort();                                                                     \
        logStream << x;                                                                                                \
        logger.flush(true);                                                                                            \
    } while (0)
