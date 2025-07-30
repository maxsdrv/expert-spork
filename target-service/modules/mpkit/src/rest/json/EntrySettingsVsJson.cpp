#include "mpk/rest/json/EntrySettingsVsJson.h"

#include "mpk/rest/tag/EntrySettingsTag.h"

#include "httpws/json/ConnectionSettings.h"

using EntrySettings = mpk::rest::EntrySettings;
using Tag = mpk::rest::EntrySettingsTag;

QJsonValue ToJsonConverter<EntrySettings>::convert(const EntrySettings& value)
{
    return QJsonObject{
        {Tag::httpServer, json::toValue(value.httpServer)},
        {Tag::broadcastServer, json::toValue(value.broadcastServer)},
    };
}

EntrySettings::ConnectionSettingsOpt loadConnectionSettings(
    const QJsonObject& object, const QString& tag)
{
    if (object.contains(tag))
    {
        return json::fromObjectWithDefault(object, tag, EntrySettings::ConnectionSettingsOpt{});
    }

    return std::nullopt;
}

EntrySettings FromJsonConverter<EntrySettings>::get(const QJsonValue& value)
{
    auto object = json::fromValue<QJsonObject>(value);

    return EntrySettings{
        .httpServer = loadConnectionSettings(object, Tag::httpServer),
        .broadcastServer = loadConnectionSettings(object, Tag::broadcastServer),
    };
}
