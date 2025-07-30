#pragma once

#include "rpc/Types.h"

#include "json/JsonComposer.h"
#include "json/JsonExceptions.h"
#include "json/JsonParser.h"
#include "json/JsonUtilities.h"

#include "qt/Strings.h"

#include <QByteArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#if QT_MAJOR > 5
#include <QCborMap>
#endif

#include <utility>

namespace rpc
{

enum class DataFormat
{
    Binary,
    Json
};

class Protocol final
{
    class Serializer;
    class Deserializer;

public:
    using BufferType = QByteArray;
    using SerializerType = Serializer;
    using DeserializerType = Deserializer;

    explicit Protocol(DataFormat format) : m_format(format)
    {
    }

    Serializer serializer()
    {
        return Serializer{m_format};
    }

    Deserializer deserializer(const QByteArray& buffer)
    {
        return Deserializer{buffer, m_format};
    }

private:
    DataFormat m_format;

private:
    class Serializer final
    {
    public:
        explicit Serializer(DataFormat format) : m_format(format)
        {
        }

        ~Serializer() noexcept = default;
        Serializer(Serializer&&) noexcept = default;
        Serializer& operator=(Serializer&&) noexcept = default;

        Serializer(const Serializer&) = delete;
        Serializer& operator=(const Serializer&) = delete;

        /**
         * Pack single value
         */
        template <typename Arg>
        Serializer& packValue(const QString& id, Arg arg)
        {
            json::putUniqueValue(m_json, id, arg);
            return *this;
        }

        /**
         * Pack array of arguments
         */
        template <typename... Args>
        Serializer& packArray(const QString& id, Args... args)
        {
            QJsonArray argsArray;
            (argsArray.push_back(json::toValue(args)), ...);
            m_json[id] = argsArray;

            return *this;
        }

        QByteArray buffer() const
        {
            QJsonDocument doc;
            doc.setObject(m_json);
            if (m_format == DataFormat::Json)
            {
                return doc.toJson();
            }
#if QT_MAJOR < 6
            return doc.toBinaryData();
#else
            QCborValue cbor(QCborMap::fromVariantMap(m_json.toVariantMap()));
            return cbor.toCbor();
#endif
        }

    private:
        QJsonObject m_json;
        DataFormat m_format;
    };

    class Deserializer final
    {
    public:
        Deserializer(Deserializer&&) noexcept = default;
        Deserializer& operator=(Deserializer&&) noexcept = default;
        ~Deserializer() noexcept = default;

        Deserializer(const Deserializer&) = delete;
        Deserializer& operator=(const Deserializer&) = delete;

        Deserializer(const QByteArray& buffer, DataFormat format)
        {
            auto error = QJsonParseError{};
            QJsonDocument document;

            if (format == DataFormat::Json)
            {
                document = QJsonDocument::fromJson(buffer, &error);
            }
            else
            {
#if QT_MAJOR < 6
                document = QJsonDocument::fromBinaryData(buffer);
#else
                QCborParserError error;
                auto cbor = QCborValue::fromCbor(buffer, &error);
                if (error.error == QCborError::NoError)
                {
                    BOOST_THROW_EXCEPTION(
                        exception::json::InvalidJson()
                        << exception::json::Error(error.errorString()));
                }
                m_json = cbor.toJsonValue().toObject();
                return;
#endif
            }

            if (document.isNull())
            {
                BOOST_THROW_EXCEPTION(
                    exception::json::InvalidJson()
                    << exception::json::Error(error.errorString()));
            }

            m_json = document.object();
        }

        // Helper function to unpack single value
        template <typename T>
        Deserializer& unpackValue(const QString& id, T& value)
        {
            value = json::fromObject<T>(m_json, id);
            return *this;
        }

        template <typename... Args>
        Deserializer& unpackArray(const QString& id, std::tuple<Args...>& tuple)
        {
            auto array = json::fromObject<QJsonArray>(m_json, id);
            if (array.size() != sizeof...(Args))
            {
                BOOST_THROW_EXCEPTION(
                    exception::json::UnexpectedValue() << exception::json::Key(id));
            }

            auto it = array.constBegin();
            unpackIndexed(it, tuple, std::make_index_sequence<sizeof...(Args)>{});
            return *this;
        }

    private:
        template <typename T>
        void unpackIndexed(
            QJsonArray::const_iterator& /*it*/,
            T& /*tuple*/,
            std::integer_sequence<std::size_t> /*seq*/)
        {
            // For empty tuple
        }

        template <typename T, std::size_t... I>
        void unpackIndexed(
            QJsonArray::const_iterator& it, T& tuple, std::index_sequence<I...> /*seq*/)
        {
            auto getItem = [&it](auto& i)
            {
                i = json::fromValue<std::decay_t<decltype(i)>>(*it);
                ++it;
            };
            (getItem(std::get<I>(tuple)), ...);
        }

    private:
        QJsonObject m_json;
    };
};

} // namespace rpc

template <>
struct FromJsonConverter<rpc::DataFormat>
{
    static rpc::DataFormat get(const QJsonValue& value)
    {
        auto tag = json::fromValue<QString>(value);
        if (tag == "json")
        {
            return rpc::DataFormat::Json;
        }
        if (tag == "binary")
        {
            return rpc::DataFormat::Binary;
        }

        BOOST_THROW_EXCEPTION(exception::json::UnexpectedValue() << exception::json::Value(tag));
    }
};

template <>
struct ToJsonConverter<rpc::ReturnCode>
{
    static QJsonValue convert(const rpc::ReturnCode& value)
    {
        return QJsonValue(static_cast<int>(value));
    }
};

template <>
struct FromJsonConverter<rpc::ReturnCode>
{
    static rpc::ReturnCode get(const QJsonValue& value)
    {
        return static_cast<rpc::ReturnCode>(FromJsonConverter<int>::get(value));
    }
};
