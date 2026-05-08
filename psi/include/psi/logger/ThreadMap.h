
#pragma once

#include <array>
#include <atomic>
#include <string>

namespace psi::logger {

/**
 * @brief Fixed-capacity map from OS thread IDs to short display names.
 *
 * Assigns a short sequential identifier (e.g. `"T0001"`) to each new thread
 * the first time it emits a log message.  Lookups and insertions are
 * lock-free — the map uses an atomic counter for slot allocation and a
 * flat array for storage (up to 5000 threads).
 */
class ThreadMap
{
    /**
     * @brief Fixed-length thread name storage (at most `MAX_THREAD_NAME_LEN` chars).
     */
    class ThreadName
    {
    public:
        /// @brief Maximum length of a stored thread name, including null terminator.
        static constexpr uint16_t MAX_THREAD_NAME_LEN = 5u;

        /// @brief Default-construct with an empty name.
        ThreadName();

        /**
         * @brief Store a name from a fixed-length char array.
         * @param in  Source array of exactly `MAX_THREAD_NAME_LEN` characters.
         */
        void setData(const char in[MAX_THREAD_NAME_LEN]);

        /// @brief Return the stored name as a `std::string`.
        std::string getData() const;

    private:
        char data[MAX_THREAD_NAME_LEN];
    };

    using ThreadInfo = std::pair<uint16_t, ThreadName>;

public:
    /// @brief Construct an empty map.
    ThreadMap();

    /**
     * @brief Return the display name for the calling thread.
     *
     * If the thread is seen for the first time it is assigned the next
     * available short name and inserted into the map.
     *
     * @return Short thread label (e.g. `"T0001"`).
     */
    std::string currentThreadName();

private:
    /// @brief Assign a new name to @p threadId and return it.
    std::string insertThread(uint16_t threadId);

private:
    std::atomic<uint16_t> m_threadLastMappedId;
    std::array<ThreadInfo, 5000u> m_data;
};

} // namespace psi::logger
