#include "qt/UnixSignalHandler.h"

#include "gsl/assert"

#include <QCoreApplication>
#include <QSocketNotifier>

#include <csignal>

#include <sys/socket.h>
#include <unistd.h>

namespace appkit
{

UnixSignalHandler::UnixSignalHandler(int signal, QObject* parent) : QObject(parent)
{
    Expects(handlers()[signal] == nullptr);

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, m_sockets) != 0)
    {
        return;
    }

    m_notifier = new QSocketNotifier(m_sockets[1], QSocketNotifier::Read, this);
    connect(m_notifier, &QSocketNotifier::activated, this, &UnixSignalHandler::read);

    std::signal(signal, &UnixSignalHandler::handle);

    handlers()[signal] = this;
}

void UnixSignalHandler::read(int socket)
{
    m_notifier->setEnabled(false);
    char tmp; // NOLINT
    if (::read(socket, &tmp, sizeof(tmp)) != 0)
    {
        emit raised();
        m_notifier->setEnabled(true);
    }
}

std::array<UnixSignalHandler*, UnixSignalHandler::max_signal>& UnixSignalHandler::handlers()
{
    static auto handlers_ =
        std::array<UnixSignalHandler*, UnixSignalHandler::max_signal>{nullptr};
    return handlers_;
}

void UnixSignalHandler::handle(int signal)
{
    if (const auto* const signalHandler = handlers()[signal]; signalHandler != nullptr)
    {
        char c = 1;
        [[maybe_unused]] auto result = ::write(signalHandler->m_sockets[0], &c, sizeof(c));
    }
}

void installUnixSignalHandler(gsl::not_null<QCoreApplication*> app)
{
#ifdef SIGINT
    QObject::connect(
        new UnixSignalHandler(SIGINT, app),
        &UnixSignalHandler::raised,
        app,
        &QCoreApplication::quit,
        Qt::QueuedConnection);
#endif
#ifdef SIGTERM
    QObject::connect(
        new UnixSignalHandler(SIGTERM, app),
        &UnixSignalHandler::raised,
        app,
        &QCoreApplication::quit,
        Qt::QueuedConnection);
#endif
}

} // namespace appkit
