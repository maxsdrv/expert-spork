#include "httpws/ws/WebSocketClientImpl.h"

#include "exception/DuplicateEntity.h"
#include "exception/ErrorInfo.h"
#include "exception/MissingEntity.h"

#include "json/JsonParser.h"
#include "json/JsonUtilities.h"
#include "log/Log2.h"
#include "qttypes/QStringConverters.h"

#include <QWebSocket>

namespace http
{

WebSocketClientImpl::WebSocketClientImpl(
    QWebSocket* socket, QObject* parent, bool handleAllMessages) :
  QObject(parent), m_socket(socket), m_handleAllMessages(handleAllMessages)
{
    QObject::connect(
        m_socket,
        &QWebSocket::binaryMessageReceived,
        this,
        [this](const auto& message) { handleMessage(message); });

    QObject::connect(
        m_socket,
        &QWebSocket::textMessageReceived,
        this,
        [this](const QString& message) { handleMessage(message.toUtf8()); });
}

void WebSocketClientImpl::registerHandler(MessageHandler handler)
{
    m_handlerAllMessages = std::move(handler);
}

void WebSocketClientImpl::registerHandler(const QString& type, MessageHandler handler)
{
    if (auto it = m_handlers.find(type); it != m_handlers.end())
    {
        BOOST_THROW_EXCEPTION(
            exception::DuplicateEntity() << exception::ExceptionInfo(strings::toUtf8(type)));
    }

    m_handlers.emplace(type, std::move(handler));
}

void WebSocketClientImpl::unregisterHandler(const QString& type)
{
    m_handlers.erase(type);
}

void WebSocketClientImpl::unregisterHandler()
{
    m_handlerAllMessages = MessageHandler{};
}

void WebSocketClientImpl::handleMessage(const QByteArray& message)
{
    try
    {
        handleJson(json::fromByteArray(message));
    }
    catch (const std::exception& ex)
    {
        LOG_ERROR << "WebSocket message handling failed: " << ex.what();
    }
}

void WebSocketClientImpl::handleJson(const QJsonObject& json)
{
    if (m_handlerAllMessages)
    {
        m_handlerAllMessages(json);
    }

    if (m_handlers.empty())
    {
        return;
    }

    auto type = json::fromObject<QString>(json, "type");
    auto it = m_handlers.find(type);
    if (it == m_handlers.end() && !m_handlerAllMessages)
    {
        if (m_handleAllMessages)
        {
            BOOST_THROW_EXCEPTION(
                exception::MissingEntity() << exception::ExceptionInfo(strings::toUtf8(type)));
        }
        else
        {
            return;
        }
    }

    it->second(json);
}

} // namespace http
