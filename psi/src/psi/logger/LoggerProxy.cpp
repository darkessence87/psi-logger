#include "psi/logger/LoggerProxy.h"

namespace psi::logger {

LoggerProxy::LoggerProxy()
    : LoggerClient(nullptr)
{
}

void LoggerProxy::log(uint16_t clientId, std::string msg)
{
    const size_t MAX_MSG_SZ = 479;
    auto splitMsg = [=](const std::string &str) -> std::vector<std::string> {
        if (str.length() <= MAX_MSG_SZ) {
            return {str};
        }

        std::vector<std::string> result;
        size_t startOffset = 0;
        while (startOffset < str.length()) {
            size_t endOffset = startOffset + MAX_MSG_SZ;
            endOffset = endOffset > str.length() ? str.length() : endOffset;
            result.emplace_back(std::string(str.begin() + startOffset, str.begin() + endOffset) + "\n");
            startOffset += MAX_MSG_SZ;
        }

        return result;
    };

    for (const auto &msgPart : splitMsg(msg)) {
        INVOKE_SERVER_FN(0x02, clientId, msgPart);
    }
}

} // namespace psi::logger