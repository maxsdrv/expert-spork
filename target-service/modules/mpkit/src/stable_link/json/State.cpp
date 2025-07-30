#include "mpk/stable_link/json/State.h"

#include "mpk/stable_link/tag/StateTag.h"

#include "json/JsonComposer.h"
#include "json/JsonParser.h"

using namespace mpk::stable_link;
using Tag = StateTag;

QJsonValue ToJsonConverter<State>::convert(State state)
{
    switch (state)
    {
        case State::OFFLINE: return Tag::offline;
        case State::DISCONNECTED: return Tag::disconnected;
        case State::CONNECTING: return Tag::connecting;
        case State::CONNECTED: return Tag::connected;

        default:
            BOOST_THROW_EXCEPTION(std::invalid_argument{
                "Unexpected State value: " + std::to_string(static_cast<int>(state))});
    }
}

State FromJsonConverter<State>::get(const QJsonValue& value)
{
    const auto string = json::fromValue<std::string>(value);

    if (string == Tag::offline)
    {
        return State::OFFLINE;
    }
    if (string == Tag::disconnected)
    {
        return State::DISCONNECTED;
    }
    if (string == Tag::connecting)
    {
        return State::CONNECTING;
    }
    if (string == Tag::connected)
    {
        return State::CONNECTED;
    }

    BOOST_THROW_EXCEPTION(exception::json::UnexpectedValue{QString::fromStdString(string)});
}
