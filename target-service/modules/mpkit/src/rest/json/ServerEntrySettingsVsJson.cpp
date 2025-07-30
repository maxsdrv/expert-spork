#include "mpk/rest/json/ServerEntrySettingsVsJson.h"

#include "mpk/rest/tag/ServerEntrySettingsTag.h"

#include "mpk/rest/SemanticVersionUtility.h"

using ServerEntrySettings = mpk::rest::ServerEntrySettings;
using SemanticVersionUtility = mpk::rest::SemanticVersionUtility;
using Tag = mpk::rest::ServerEntrySettingsTag;

QJsonValue ToJsonConverter<ServerEntrySettings>::convert(const ServerEntrySettings& value)
{

    auto jsonAddress = QJsonObject{
        {Tag::host, json::toValue(value.httpAddress.host)},
        {Tag::httpPort, json::toValue(value.httpAddress.port)},
    };

    if (value.broadcastAddress.has_value())
    {
        jsonAddress.insert(Tag::broadcastPort, value.broadcastAddress->port);
    }

    QJsonObject result{
        {Tag::address, jsonAddress},
        {Tag::login, json::toValue(value.login)},
    };

    if (value.requiredVersion.has_value())
    {
        auto versionStr = SemanticVersionUtility::toString(*value.requiredVersion);
        result.insert(Tag::requiredVersion, json::toValue(versionStr));
    }

    return result;
}

ServerEntrySettings FromJsonConverter<ServerEntrySettings>::get(const QJsonValue& value)
{
    auto object = json::fromValue<QJsonObject>(value);

    auto jsonAddress = json::fromValue<QJsonObject>(object.value(Tag::address));

    mpk::rest::ServerEntrySettings restEntry;

    restEntry.httpAddress.host = json::fromObject<QString>(jsonAddress, Tag::host);
    restEntry.httpAddress.port = json::fromObject<int>(jsonAddress, Tag::httpPort);

    restEntry.login = json::fromObject<QString>(object, Tag::login);

    if (jsonAddress.contains(Tag::broadcastPort))
    {
        restEntry.broadcastAddress = restEntry.httpAddress;
        restEntry.broadcastAddress->port = json::fromObject<int>(jsonAddress, Tag::broadcastPort);
    }

    if (object.contains(Tag::requiredVersion))
    {
        auto versionStr = json::fromObject<std::string>(object, Tag::requiredVersion);

        restEntry.requiredVersion = SemanticVersionUtility::fromString(versionStr);
    }

    return restEntry;
}
