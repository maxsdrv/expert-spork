#pragma once

#include "mpk/rest/Client.h"
#include "mpk/rest/ConditionWrapper.h"
#include "mpk/rest/ErrorInfo.h"

#include "json/JsonComposer.h"
#include "json/JsonParser.h"

#include "httpws/HttpUtils.h"

#include "gsl/pointers"

#include <QFuture>
#include <QtConcurrentRun>

#include <memory>

namespace mpk::rest
{

class RestApiClientHelper
{
public:
    using Client = mpk::rest::Client;
    using ClientSingle = gsl::not_null<const Client*>;

    using ConditionObject = mpk::rest::ConditionWrapper<QJsonObject>;
    using ConditionObjectShared = std::shared_ptr<ConditionObject>;

    explicit RestApiClientHelper(ClientSingle restClient);

    template <class ResultType>
    QFuture<ResultType> get(const QString& path, const QJsonObject& params) const
    {
        return runQuery<ResultType>(path, params, &Client::get);
    }

    template <class ResultType>
    QFuture<ResultType> put(const QString& path, const QJsonObject& params) const
    {
        return runQuery<ResultType>(path, params, &Client::put);
    }

private:
    template <
        class ResultType,

        class ResultObject = typename ResultType::ResultObject,
        class HttpResultType = typename std::
            conditional_t<std::is_same_v<ResultObject, std::string>, std::string, QJsonObject>,

        class ResultHandler = std::function<void(const HttpResultType& result)>,
        class ErrorCodeHandler = std::function<void(int errorCode)>>

    QFuture<ResultType> runQuery(
        const QString& path,
        const QJsonObject& params,
        void (Client::*query)(const QString&, const QJsonObject&, ResultHandler, ErrorCodeHandler)
            const) const
    {
        auto conditionObject = std::make_shared<ConditionObject>();

        (m_restClient->*query)(
            path,
            params,
            [conditionObject](const HttpResultType& httpResult)
            {
                if constexpr (std::is_same_v<HttpResultType, QJsonObject>)
                {
                    conditionObject->setResult(httpResult);
                }
                else
                {
                    conditionObject->setResult(
                        QJsonObject{{textDataTag().c_str(), httpResult.c_str()}});
                }
            },
            [conditionObject](int errorCode)
            {
                const QJsonObject object{{errorCodeTag().c_str(), errorCode}};
                conditionObject->setResult(object);
            });

        return waitResult<ResultType>(conditionObject);
    }

    template <class ResultType>
    static QFuture<ResultType> waitResult(const ConditionObjectShared& conditionObject)
    {
        return QtConcurrent::run(
            [conditionObject]()
            {
                const auto& json = conditionObject->waitResult();
                if (json.contains(errorCodeTag().c_str()))
                {
                    return handleError<ResultType>(json);
                }

                return handleResult<ResultType>(json);
            });
    }

    template <class ResultType>
    static ResultType handleError(const QJsonObject& json)
    {
        const auto errorCode = json.value(errorCodeTag().c_str()).toInt();

        const auto readError = 4;
        // library httplib has the same error httplib::Error::Read
        // for timeout and read error. In almost cases when load
        // tracks it means timeout
        QString errorMessage;
        if (errorCode == readError)
        {
            errorMessage = QString::fromStdString("Timeout");
        }
        else
        {
            errorMessage = http::errorToString(errorCode);
        }

        const ErrorInfo errorInfo{.message = errorMessage, .code = errorCode};

        ResultType result{};
        result.error = errorInfo;

        return result;
    }

    template <class ResultType>
    static ResultType handleResult(const QJsonObject& json)
    {
        std::optional<ErrorInfo> error;
        if constexpr (std::is_same_v<ResultType, QJsonObject>)
        {
            return ResultType{json, error};
        }
        else
        {
            using ResultObject = typename ResultType::ResultObject;

            if constexpr (std::is_same_v<ResultObject, std::string>)
            {
                auto data = json.value(textDataTag().c_str()).toString();
                return ResultType{data.toStdString(), error};
            }
            else
            {
                auto resultObject = json::fromValue<ResultObject>(json);
                return ResultType{resultObject, error};
            }
        }
    }

    static std::string errorCodeTag();
    static std::string textDataTag();

private:
    ClientSingle m_restClient;
};

} // namespace mpk::rest
