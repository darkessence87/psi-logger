
#include "psi/logger/LoggerProxy.h"

namespace psi::logger {

LoggerProxy::LoggerProxy()
    : LoggerClient(nullptr)
{
}

void LoggerProxy::log(uint16_t clientId, std::string msg)
{
    constexpr size_t MAX_MSG_SZ = 479;

    size_t start = 0;
    const size_t len = msg.size();

    while (start < len) {
        const size_t chunkSize = std::min(MAX_MSG_SZ, len - start);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
        std::string_view part {msg.data() + start, chunkSize};
#pragma clang diagnostic pop

        // Добавляем \n только если это не последний кусок
        if (start + chunkSize < len) {
            std::string tmp;
            tmp.reserve(chunkSize + 1);
            tmp.append(part);
            tmp.push_back('\n');
            INVOKE_SERVER_FN(0x02, clientId, tmp);
        } else {
            INVOKE_SERVER_FN(0x02, clientId, part);
        }

        start += chunkSize;
    }
}

} // namespace psi::logger
