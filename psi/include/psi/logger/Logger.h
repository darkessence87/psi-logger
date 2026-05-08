#pragma once

#include "LoggerBase.h"

/// @brief Log a DEBUG message from an instance method (captures `this` address).
#define LOG_DEBUG(x) LOG_MSG(x, psi::logger::LogLevel::LVL_DEBUG)

/// @brief Log an ERROR message from an instance method.
#define LOG_ERROR(x) LOG_MSG(x, psi::logger::LogLevel::LVL_ERROR)

/// @brief Log an INFO message from an instance method.
#define LOG_INFO(x) LOG_MSG(x, psi::logger::LogLevel::LVL_INFO)

/// @brief Log a WARNING message from an instance method.
#define LOG_WARN(x) LOG_MSG(x, psi::logger::LogLevel::LVL_WARNING)

/// @brief Log a TRACE message from an instance method.
#define LOG_TRACE(x) LOG_MSG(x, psi::logger::LogLevel::LVL_TRACE)

/// @brief Log a DEBUG message from a static or free function (address reported as 0).
#define LOG_DEBUG_STATIC(x) LOG_MSG_STATIC(x, psi::logger::LogLevel::LVL_DEBUG)

/// @brief Log an ERROR message from a static or free function.
#define LOG_ERROR_STATIC(x) LOG_MSG_STATIC(x, psi::logger::LogLevel::LVL_ERROR)

/// @brief Log an INFO message from a static or free function.
#define LOG_INFO_STATIC(x) LOG_MSG_STATIC(x, psi::logger::LogLevel::LVL_INFO)

/// @brief Log a WARNING message from a static or free function.
#define LOG_WARN_STATIC(x) LOG_MSG_STATIC(x, psi::logger::LogLevel::LVL_WARNING)

/// @brief Log a TRACE message from a static or free function.
#define LOG_TRACE_STATIC(x) LOG_MSG_STATIC(x, psi::logger::LogLevel::LVL_TRACE)
