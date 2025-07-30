#pragma once

#include <QJsonObject>

#include <functional>

namespace mpk::rest
{

class OperationCaller
{
public:
    using JsonResultHandler = std::function<void(const QJsonObject& object)>;
    using TextResultHandler = std::function<void(const QString& text)>;
    using ErrorCodeHandler = std::function<void(int errorCode)>;

    virtual ~OperationCaller() = default;

    virtual void get(const QString& operation, JsonResultHandler resultHandler) const = 0;
    virtual void get(
        const QString& operation,
        const QJsonObject& parameters,
        JsonResultHandler resultHandler) const = 0;
    virtual void get(
        const QString& operation,
        const QJsonObject& parameters,
        JsonResultHandler resultHandler,
        ErrorCodeHandler errorHandler) const = 0;
    virtual void get(
        const QString& operation,
        const QJsonObject& parameters,
        TextResultHandler resultHandler,
        ErrorCodeHandler errorHandler) const = 0;

    virtual void post(const QString& operation, const QJsonObject& parameters) const = 0;
    virtual void post(
        const QString& operation,
        const QJsonObject& parameters,
        ErrorCodeHandler errorHandler) const = 0;
    virtual void post(
        const QString& operation,
        const QJsonObject& parameters,
        JsonResultHandler resultHandler,
        ErrorCodeHandler errorHandler) const = 0;

    virtual void put(const QString& operation, const QJsonObject& parameters) const = 0;
    virtual void put(
        const QString& operation,
        const QJsonObject& parameters,
        JsonResultHandler resultHandler) const = 0;
    virtual void put(
        const QString& operation,
        const QJsonObject& parameters,
        JsonResultHandler resultHandler,
        ErrorCodeHandler errorHandler) const = 0;

    virtual void del(
        const QString& operation,
        const QJsonObject& parameters,
        ErrorCodeHandler errorHandler) const = 0;
};

} // namespace mpk::rest
