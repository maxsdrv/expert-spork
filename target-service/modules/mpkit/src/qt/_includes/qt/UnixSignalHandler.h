#pragma once

#include "gsl/pointers"

#include <QObject>

#include <array>

class QCoreApplication;
class QSocketNotifier;

namespace appkit
{

class UnixSignalHandler : public QObject
{
    Q_OBJECT

public:
    explicit UnixSignalHandler(int signal, QObject* parent = nullptr);

signals:
    void raised();

private slots:
    void read(int socket);

private:
    static const int max_signal = 32;
    static std::array<UnixSignalHandler*, max_signal>& handlers();

    // NOLINTNEXTLINE
    int m_sockets[2] = {0, 0};
    QSocketNotifier* m_notifier = nullptr;

    static void handle(int signal);
};

void installUnixSignalHandler(gsl::not_null<QCoreApplication*> app);

} // namespace appkit
