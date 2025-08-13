#pragma once

#include <QNetworkReply>
#include <QString>

#include <chrono>
#include <utility>

namespace mpk::dss::api
{

void handleRequestFailure(
    const QString& requestName,
    bool& httpTimeout,
    bool& httpError,
    QNetworkReply::NetworkError errorType,
    const QString& errorStr);

void handleRequestFailure(
    const QString& requestName,
    bool& httpTimeout,
    bool& httpError,
    int errorCode);

std::string timeToStdString(std::chrono::system_clock::time_point t_point);

} // namespace mpk::dss::api
