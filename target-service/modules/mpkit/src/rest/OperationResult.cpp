#include "mpk/rest/OperationResult.h"

#include "qt/MetaEnum.h"

namespace mpk::rest
{

OperationResult::OperationResult(QObject* parent) : QObject(parent)
{
}

OperationResult::Value OperationResult::fromQString(const QString& string)
{
    return qt::metaEnumFromString<Value>(string);
}

QString OperationResult::toQString(const Value& value)
{
    return qt::metaEnumToString(value);
}

QString OperationResult::toDisplayString(Value value)
{
    switch (value)
    {
        case Value::UNKNOWN: return tr("Unknown");
        case Value::SUCCESS: return tr("Success");
        case Value::UNAUTHORIZED: return tr("Unauthorized");
        case Value::NOT_PERMITTED: return tr("Not permitted");
        default: return tr("Invalid value");
    }
}

} // namespace mpk::rest
