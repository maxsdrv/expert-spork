#include "ChannelStatus.h"

namespace mpk::drone::core
{

std::ostream& operator<<(std::ostream& stream, ChannelStatus status)
{
    switch (status)
    {
        case ChannelStatus::ALARM:
            stream << "ALARM";
            break;
        case ChannelStatus::NO_ALARM:
            stream << "NO_ALARM";
            break;
        case ChannelStatus::SOFTWARE_ERROR:
            stream << "SOFTWARE_ERROR";
            break;
        default:
            stream << "Undefined";
            break;
    }

    return stream;
}

} // namespace mpk::drone::core
