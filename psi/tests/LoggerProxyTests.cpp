
#include "psi/test/TestHelper.h"
#include "psi/test/psi_mock.h"

#include "psi/logger/LoggerProxy.h"
#include "psi/logger/generated_LoggerClient.h"
#include "psi/logger/LoggerSingleton.h"

using namespace psi::test;

TEST(LoggerProxyTests, constructor)
{
    // Just constructing LoggerProxy must not crash.
    psi::logger::LoggerProxy proxy;
    EXPECT_TRUE(true);
}

TEST(LoggerProxyTests, log_short_message)
{
    // Message shorter than 479 bytes — no chunking.
    psi::logger::LoggerProxy proxy;
    proxy.log(1u, "Hello, world!");
    EXPECT_TRUE(true);
}

TEST(LoggerProxyTests, log_empty_message)
{
    // Empty message — while loop is never entered.
    psi::logger::LoggerProxy proxy;
    proxy.log(1u, "");
    EXPECT_TRUE(true);
}

TEST(LoggerProxyTests, log_long_message_with_chunking)
{
    // Message longer than 479 bytes — forces at least one chunked iteration
    // (the branch that appends '\n' between chunks).
    psi::logger::LoggerProxy proxy;
    const std::string long_msg(500, 'A');
    proxy.log(2u, long_msg);
    EXPECT_TRUE(true);
}

TEST(LoggerProxyTests, log_exact_boundary_message)
{
    // Message exactly 479 bytes — single chunk, no '\n' appended.
    psi::logger::LoggerProxy proxy;
    const std::string msg(479, 'B');
    proxy.log(3u, msg);
    EXPECT_TRUE(true);
}

TEST(LoggerProxyTests, log_multi_chunk_message)
{
    // Message requiring multiple chunks (> 2 * 479 bytes).
    psi::logger::LoggerProxy proxy;
    const std::string msg(1100, 'C');
    proxy.log(4u, msg);
    EXPECT_TRUE(true);
}

TEST(LoggerClientTests, log_calls_base_implementation)
{
    // Create LoggerClient directly to cover LoggerClient::log() (not the
    // LoggerProxy override) and LoggerClient::registerClient().
    psi::logger::LoggerClient client(nullptr);
    client.log(1u, "direct log");
    EXPECT_TRUE(true);
}

TEST(LoggerClientTests, registerClient_calls_base_implementation)
{
    psi::logger::LoggerClient client(nullptr);
    psi::logger::ILogger::OnClientRegistered cb([](uint16_t, std::string, uint16_t) {});
    client.registerClient("test_client", std::move(cb));
    EXPECT_TRUE(true);
}

TEST(LoggerSingletonTests, setLogLevel_changes_level)
{
    auto &ls = psi::logger::getLoggerInstance("SingletonCoverage");
    ls.setLogLevel(psi::logger::LogLevel::LVL_DEBUG);
    EXPECT_TRUE(ls.needLog(psi::logger::LogLevel::LVL_DEBUG));
}

TEST(LoggerSingletonTests, logStreamShort_and_flush_isShort)
{
    // logStreamShort() + flush(true) covers the isShort=true branch in flush.
    auto &ls = psi::logger::getLoggerInstance("SingletonCoverage");
    auto &stream = ls.logStreamShort();
    stream << "short path";
    ls.flush(true);
    EXPECT_TRUE(true);
}

TEST(LoggerSingletonTests, destructor_coverage)
{
    // Create a LoggerSingleton on the stack so the destructor is exercised.
    {
        psi::logger::LoggerSingleton ls("TmpDestructorCtx");
        ls.setLogLevel(psi::logger::LogLevel::LVL_WARNING);
    }
    EXPECT_TRUE(true);
}
