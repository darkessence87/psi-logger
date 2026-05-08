
#pragma once

#include "generated_LoggerClient.h"

namespace psi::logger {

/**
 * @brief IPC transport layer between `LoggerSingleton` and the logger server.
 *
 * `LoggerProxy` wraps the auto-generated `LoggerClient` and overrides `log()`
 * to transparently split messages that exceed the IPC payload limit (479 bytes)
 * into sequential chunks, each sent as a separate IPC call.
 *
 * A `LoggerProxy` instance is owned by `LoggerSingleton` and connects with
 * a `nullptr` event loop (fire-and-forget, no response required).
 */
class LoggerProxy : public LoggerClient
{
public:
    /// @brief Construct and connect to the logger IPC service.
    LoggerProxy();

    /**
     * @brief Send @p msg to the logger server, chunking if necessary.
     *
     * Messages longer than 479 bytes are split into multiple sequential
     * IPC calls.  A newline is appended between chunks so the server can
     * reconstruct the full record.
     *
     * @param arg0 Client identifier returned by `registerClient`.
     * @param arg1 Full log message to transmit.
     */
    void log(uint16_t arg0, std::string arg1) override;
};

} // namespace psi::logger
