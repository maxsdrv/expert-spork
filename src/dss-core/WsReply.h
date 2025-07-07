#pragma once

#include "httpws/HttpException.h"

#include <QObject>
#include <QString>

namespace mpk::dss::core
{

class WsPublisher;

class WsReply : public QObject
{
    Q_OBJECT

public:
    explicit WsReply(
        QString name,
        QString id,
        WsPublisher* publisher,
        bool suppressLog = false,
        QObject* parent = nullptr);
    ~WsReply() override;

    void finish(http::HttpCode code, const QString& message);
    void finish(const exception::http::Exception& ex);

private:
    void notify(http::HttpCode code, const QString& message);
    QString m_name;
    QString m_id;
    bool m_started;
    bool m_suppressLog;
    WsPublisher* m_publisher;
};

} // namespace mpk::dss::core
