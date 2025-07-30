#pragma once

#include <QString>

namespace http
{

struct ConnectionSettings
{
    struct Tag
    {
        static constexpr auto host = "host";
        static constexpr auto port = "port";
    };

    QString host;
    int port{-1};

    bool operator==(const ConnectionSettings& rhs) const;
    bool operator!=(const ConnectionSettings& rhs) const;

    static bool isValid(const ConnectionSettings& settings);
};

} // namespace http
