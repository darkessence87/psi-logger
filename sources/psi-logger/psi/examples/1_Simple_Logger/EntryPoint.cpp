#include "psi/logger/Logger.h"

int main()
{
    using namespace psi;

    /**
     * @brief Default "LOGG" application label may be overriden by calling
     * "getLoggerInstance(appName)" BEFORE any logger macro is used first time.
     * 
     */
    logger::getLoggerInstance("PSI_EXAMPLES").setLogLevel(logger::LogLevel::LVL_TRACE);

    /**
     * @brief Under the hood "test" message is saved to shared memory by LoggerClient.
     * LoggerServer(s) may connect to shared memory and read the traces.
     * Message format is:
     * timestampId << local_time << logLevel << processId << threadName << static/thisAddress << fileName << fnName << line << msg
     * Default separator: ','
     * 
     */
    LOG_TRACE_STATIC("test");
}
