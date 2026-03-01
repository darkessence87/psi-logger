
#pragma once

#include <array>
#include <atomic>
#include <string>

namespace psi::logger {

class ThreadMap
{
    class ThreadName
    {
    public:
        static constexpr uint16_t MAX_THREAD_NAME_LEN = 5u;
        ThreadName();
        void setData(const char in[MAX_THREAD_NAME_LEN]);
        std::string getData() const;

    private:
        char data[MAX_THREAD_NAME_LEN];
    };

    using ThreadInfo = std::pair<uint16_t, ThreadName>;

public:
    ThreadMap();
    std::string currentThreadName();

private:
    std::string insertThread(uint16_t threadId);

private:
    std::atomic<uint16_t> m_threadLastMappedId;
    std::array<ThreadInfo, 5000u> m_data;
};

} // namespace psi::logger
