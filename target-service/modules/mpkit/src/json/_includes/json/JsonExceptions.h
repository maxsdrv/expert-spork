#pragma once

#include "exception/General.h"
#include "qttypes/QStringConverters.h"

#include <QJsonDocument>
#include <QJsonObject>

namespace exception::json
{

class InvalidJson : public virtual General
{
};

using Key = boost::error_info<struct jsonKey_, QString>;
using Value = boost::error_info<struct jsonValue_, QString>;
using Error = boost::error_info<struct jsonError_, QString>;
using File = boost::error_info<struct jsonFile_, QString>;
using Body = boost::error_info<struct jsonBody_, QJsonObject>;

class UnexpectedValue : public virtual InvalidJson
{
public:
    [[deprecated("Prefer constructor with an arg")]] UnexpectedValue() = default;

    explicit UnexpectedValue(const QString& value)
    {
        (*this) << Value{value};
    }
};

class UnexpectedType : public virtual InvalidJson
{
public:
    [[deprecated("Prefer constructor with an arg")]] UnexpectedType() = default;

    explicit UnexpectedType(const QString& error)
    {
        (*this) << Error{error};
    }

    [[deprecated("Prefer constructor with an expected type")]] explicit UnexpectedType(
        QJsonValue::Type type)
    {
        (*this) << Error{QString("Unexpected type %1").arg(toString(type))};
    }

    UnexpectedType(const QSet<QJsonValue::Type>& expectedTypes, QJsonValue::Type currentType) :
      UnexpectedType{toString(expectedTypes), toString(currentType)}
    {
    }

    UnexpectedType(QJsonValue::Type expectedType, QJsonValue::Type currentType) :
      UnexpectedType{toString(expectedType), toString(currentType)}
    {
    }

    static QString toString(QJsonValue::Type type)
    {
        switch (type)
        {
            case QJsonValue::Null: return "QJsonValue::Null";
            case QJsonValue::Bool: return "QJsonValue::Bool";
            case QJsonValue::Double: return "QJsonValue::Double";
            case QJsonValue::String: return "QJsonValue::String";
            case QJsonValue::Array: return "QJsonValue::Array";
            case QJsonValue::Object: return "QJsonValue::Object";
            case QJsonValue::Undefined: return "QJsonValue::Undefined";
            default: return QString::number(type);
        }
    }

    static QString toString(const QSet<QJsonValue::Type>& expectedTypes)
    {
        QString result;
        for (const auto type: expectedTypes)
        {
            if (!result.isEmpty())
            {
                result += "|";
            }

            result += toString(type);
        }

        return result;
    }

private:
    UnexpectedType(const QString& expectedTypes, const QString& currentType)
    {
        (*this) << Error{QString("Expected %1, got %2").arg(expectedTypes, currentType)};
    }
};

class DuplicateField : public virtual InvalidJson
{
public:
    [[deprecated("Prefer constructor with an arg")]] DuplicateField() = default;

    explicit DuplicateField(const QString& key)
    {
        (*this) << Key{key};
    }
};

class MissingNode : public virtual InvalidJson
{
public:
    [[deprecated("Prefer constructor with an arg")]] MissingNode() = default;

    explicit MissingNode(const QString& key)
    {
        (*this) << Key{key};
    }
};

class InvalidValue : public virtual InvalidJson
{
};

} // namespace exception::json

inline std::ostream& operator<<(std::ostream& stream, const QJsonObject& object)
{
    stream << QJsonDocument(object).toJson(QJsonDocument::JsonFormat::Indented).toStdString();
    return stream;
}
