#pragma once

namespace mpk::rest
{

struct ServerEntrySettingsTag
{
    static constexpr auto address = "address";

    static constexpr auto host = "host";
    static constexpr auto httpPort = "http_port";
    static constexpr auto login = "login";

    static constexpr auto broadcastPort = "broadcast_port";

    static constexpr auto requiredVersion = "required_version";
};

} // namespace mpk::rest
