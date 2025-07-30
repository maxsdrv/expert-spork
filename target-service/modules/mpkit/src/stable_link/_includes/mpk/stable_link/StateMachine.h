#pragma once

#include <QFinalState>
#include <QStateMachine>

namespace mpk::stable_link
{

class StateMachine : public QStateMachine
{
    Q_OBJECT

public:
    explicit StateMachine(QObject* parent = nullptr);
    StateMachine(const StateMachine& other) = delete;
    StateMachine& operator=(const StateMachine& other) = delete;

    QState* offline() const;
    QState* online() const;
    QFinalState* onlineFinal() const;

    QState* connecting() const;
    QState* connected() const;
    QState* disconnecting() const;

    QState* responding() const;
    QState* notResponding() const;

    QState* delayConnecting() const;
    QState* reconnecting() const;
    QState* aborting() const;

    QState* connectionFailed() const;

    void switchToOnline();
    void switchToOffline();
    void switchToReconnecting();

    void addSwitchToOfflineTransition(QState* source, QAbstractState* target) const;
    void addSwitchToReconnectingTransition(QState* source, QAbstractState* target) const;

protected:
    void connectTrace();
    static void connectTrace(QAbstractState* state, const QString& name);

    // Signals are for internal use only. Don't connect.
signals:
    void switchingToOffline();
    void switchingToOnline();
    void switchingToReconnecting();

private:
    QState* m_offline;
    QState* m_online;
    QFinalState* m_onlineFinal;

    QState* m_connecting;
    QState* m_connected;
    QState* m_disconnecting;

    QState* m_responding;
    QState* m_notResponding;

    QState* m_delayConnecting;
    QState* m_reconnecting;
    QState* m_aborting;

    QState* m_connectionFailed;
};

} // namespace mpk::stable_link
