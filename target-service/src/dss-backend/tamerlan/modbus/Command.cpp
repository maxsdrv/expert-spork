#include "Command.h"

#include <tuple>

namespace mpk::dss::backend::tamerlan
{

bool operator==(const Command& l, const Command& r)
{
    return std::tie(l.type, l.address, l.numRegs, l.serverAddress)
           == std::tie(r.type, r.address, r.numRegs, r.serverAddress);
}

} // namespace mpk::dss::backend::tamerlan
