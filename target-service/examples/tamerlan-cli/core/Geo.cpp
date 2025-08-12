#include "Geo.h"

#include <tuple>

namespace mpk::drone::core
{

bool operator==(const Position& left, const Position& right)
{
    return std::tie(left.latitude, left.longitude)
           == std::tie(right.latitude, right.longitude);
}

bool operator!=(const Position& left, const Position& right)
{
    return !(left == right);
}

} // namespace mpk::drone::core
