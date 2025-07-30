#pragma once

#include "ToJson.h"

#include <QJsonObject>

namespace json
{

class JsonObjectBuilder
{
public:
    JsonObjectBuilder() = default;

    // There are cases when we first convert some object to json using existing converter
    // and then mix other fields
    template <typename T>
    explicit JsonObjectBuilder(const T& value) : m_object{json::toValue(value).toObject()}
    {
    }

    template <typename T>
    JsonObjectBuilder& add(const QString& key, const T& value)
    {
        Q_ASSERT(!m_object.contains(key));
        m_object[key] = toValue(value);
        return *this;
    }

    // NOLINTNEXTLINE11
    operator QJsonObject() const
    {
        return m_object;
    }

    template <typename T>
    T as() const
    {
        return m_object;
    }

private:
    QJsonObject m_object;
};

} // namespace json
