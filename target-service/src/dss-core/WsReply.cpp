#include "dss-core/WsReply.h"
#include "dss-core/Domains.h"
#include "dss-core/WsPublisher.h"

#include "log/Log2.h"

#include "OAServiceWs_reply.h"

namespace mpk::dss::core
{

WsReply::WsReply(
    QString name,
    QString id,
    WsPublisher* publisher,
    bool suppressLog,
    QObject* parent) :
  QObject(parent),
  m_name(std::move(name)),
  m_id(std::move(id)),
  m_started(true),
  m_suppressLog(suppressLog),
  m_publisher(publisher)
{
}

void WsReply::notify(http::HttpCode code, const QString& message)
{
    if (!m_suppressLog)
    {
        LOG_DEBUG << "WsReply: " << m_name.toStdString() << " "
                  << static_cast<int>(code) << " " << message.toStdString();
    }

    OAService::OAServiceWs_reply ws_reply;
    ws_reply.setId(m_id);
    ws_reply.setReplyText(QString("%1: %2").arg(m_name, message));
    ws_reply.setHttpCode(static_cast<int>(code));
    m_publisher->sendNotification(
        Domain::General, m_name, ws_reply.asJsonObject());
}

void WsReply::finish(http::HttpCode code, const QString& message)
{
    m_started = false;
    notify(code, message);
}

void WsReply::finish(const exception::http::Exception& ex)
{
    m_started = false;
    notify(
        exception::info<exception::http::ResponseStatus>(
            ex, http::HttpCode::InternalServerError),
        QString::fromStdString(
            exception::info<exception::http::ResponseMessage>(
                ex, std::string("Unknown error"))));
}

WsReply::~WsReply()
{
    if (m_started)
    {
        LOG_ERROR << "WsReply: Reply " << m_name.toStdString()
                  << " destroyed while still active";
        finish(http::HttpCode::InternalServerError, "Abnormal request finish");
    }
}

} // namespace mpk::dss::core
