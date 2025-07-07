#include "Helpers.h"

#include "log/Log2.h"

#include <httplib.h>

namespace mpk::dss::api
{

void handleRequestFailure(
    const QString& requestName,
    bool& httpTimeout,
    bool& httpError,
    QNetworkReply::NetworkError errorType,
    const QString& errorStr)
{
    if (errorType == QNetworkReply::TimeoutError)
    {
        httpTimeout = true;
    }
    else
    {
        httpError = true;
    }
    LOG_WARNING << "Error happened while issuing " << requestName.toStdString()
                << " request: " << errorStr.toStdString();
}

void handleRequestFailure(
    const QString& requestName,
    bool& httpTimeout,
    bool& httpError,
    int errorCode)
{
    auto error = static_cast<httplib::Error>(errorCode);
    if (error == httplib::Error::ConnectionTimeout)
    {
        httpTimeout = true;
    }
    else
    {
        httpError = true;
    }
    LOG_WARNING << "Error happened while issuing " << requestName.toStdString()
                << " request: " << httplib::to_string(error);
}

std::string timeToStdString(std::chrono::system_clock::time_point t_point)
{
    using namespace std;
    time_t timet = chrono::system_clock::to_time_t(t_point);
    return (std::ostringstream()
            << put_time(gmtime(&timet), "%Y-%m-%dT%H:%M:%SZ"))
        .str();
}
} // namespace mpk::dss::api
