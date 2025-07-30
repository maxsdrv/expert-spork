#pragma once

#include <QObject>

namespace mpk::rest
{

// TODO: A duplicate of the rctcore::OperationResult,
// take it to the common library.
class OperationResult : public QObject
{
    Q_OBJECT

public:
    explicit OperationResult(QObject* parent = nullptr);

    enum Value
    {
        UNKNOWN,
        SUCCESS,
        UNAUTHORIZED,
        NOT_PERMITTED
    };
    Q_ENUM(Value)

    Q_INVOKABLE static OperationResult::Value fromQString(const QString& string);
    Q_INVOKABLE static QString toQString(const OperationResult::Value& value);
    Q_INVOKABLE static QString toDisplayString(OperationResult::Value value);
};

} // namespace mpk::rest
