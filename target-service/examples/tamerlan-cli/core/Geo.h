#pragma once

namespace mpk::drone::core
{

struct Position
{
    double latitude;
    double longitude;
};

bool operator==(const Position& left, const Position& right);
bool operator!=(const Position& left, const Position& right);

using Angle = double;
using Distance = double;

} // namespace mpk::drone::core
