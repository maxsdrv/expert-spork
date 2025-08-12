#pragma once

#include <iostream>

namespace mpk::dss
{

struct Settings
{
    int timeout = 5;
    int httpPort = 22080;
    int websocketPort = 22081;
};

} // namespace mpk::dss
