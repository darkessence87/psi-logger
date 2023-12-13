
#include "psi/logger/IThreadMap.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string.h>
#include <thread>

namespace psi::logger {

template <size_t MAX_THREAD_NAME_LEN>
IThreadMap<MAX_THREAD_NAME_LEN>::ThreadName::ThreadName()
    : data("")
{
}

template <size_t MAX_THREAD_NAME_LEN>
void IThreadMap<MAX_THREAD_NAME_LEN>::ThreadName::setData(const char in[MAX_THREAD_NAME_LEN])
{
    memcpy(data, in, MAX_THREAD_NAME_LEN);
}

template <size_t MAX_THREAD_NAME_LEN>
std::string IThreadMap<MAX_THREAD_NAME_LEN>::ThreadName::getData() const
{
    return data;
}

template <size_t MAX_THREAD_NAME_LEN>
IThreadMap<MAX_THREAD_NAME_LEN>::IThreadMap()
    : m_threadLastMappedId(0)
{
}

template <size_t MAX_THREAD_NAME_LEN>
std::string IThreadMap<MAX_THREAD_NAME_LEN>::currentThreadName()
{
#ifdef _WIN32
    const uint16_t threadId = static_cast<uint16_t>(GetCurrentThreadId());
#else
    const uint16_t threadId = static_cast<uint16_t>(pthread_self());
#endif

    const auto firstIndex = m_threadLastMappedId ? m_data.size() - m_threadLastMappedId : m_data.size() - 1;
    const uint16_t firstThreadId = m_data.at(firstIndex).first;
    const uint16_t lastThreadId = m_data.back().first;
    if (m_threadLastMappedId == 0 || threadId < firstThreadId || threadId > lastThreadId) {
        return insertThread(threadId);
    }

    for (auto index = firstIndex; index < m_data.size(); ++index) {
        const auto &indexData = m_data.at(index);
        if (indexData.first == threadId) {
            return indexData.second.getData();
        }
    }

    return insertThread(threadId);
}

template <size_t MAX_THREAD_NAME_LEN>
std::string IThreadMap<MAX_THREAD_NAME_LEN>::insertThread(uint16_t threadId)
{
    const int64_t len = MAX_THREAD_NAME_LEN - 1;
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(len) << ++m_threadLastMappedId;

    const auto threadName = ss.str();

    m_data[m_threadLastMappedId].first = threadId;
    m_data[m_threadLastMappedId].second.setData(threadName.c_str());

    std::sort(m_data.begin(), m_data.end(), [](const ThreadInfo &t1, const ThreadInfo &t2) {
        return t1.first < t2.first;
    });

    return threadName;
}

} // namespace psi::logger
