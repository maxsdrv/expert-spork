#include "httpws/json/ConnectionSettings.h"

#include "json/JsonObjectBuilder.h"
#include "json/ObjectParser.h"
#include "json/composers/General.h"
#include "json/parsers/General.h"

QJsonValue ToJsonConverter<http::ConnectionSettings>::convert(
    const http::ConnectionSettings& settings)
{
    using Tag = http::ConnectionSettings::Tag;
    return json::JsonObjectBuilder{}
        .add(Tag::host, settings.host)
        .add(Tag::port, settings.port)
        .as<QJsonValue>();
}

http::ConnectionSettings FromJsonConverter<http::ConnectionSettings>::get(const QJsonValue& value)
{
    using Tag = http::ConnectionSettings::Tag;
    auto parser = json::ObjectParser{value};
    return http::ConnectionSettings{
        parser.get<decltype(http::ConnectionSettings::host)>(Tag::host),
        parser.get<decltype(http::ConnectionSettings::port)>(Tag::port)};
}
