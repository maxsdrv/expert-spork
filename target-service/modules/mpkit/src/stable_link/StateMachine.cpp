#include "mpk/stable_link/StateMachine.h"

#include "log/Log2.h"
#include "qt/Strings.h"

namespace mpk::stable_link
{

StateMachine::StateMachine(QObject* parent) :
  QStateMachine{parent},
  m_offline{new QState{this}},
  m_online{new QState{this}},
  m_onlineFinal{new QFinalState{m_online}},
  m_connecting{new QState{m_online}},
  m_connected{new QState{m_online}},
  m_disconnecting{new QState{m_online}},
  m_responding{new QState{m_connected}},
  m_notResponding{new QState{m_connected}},
  m_delayConnecting{new QState{m_online}},
  m_reconnecting{new QState{m_online}},
  m_aborting{new QState{m_online}},
  m_connectionFailed{new QState{m_online}}
{
    setInitialState(m_offline);

    m_offline->addTransition(this, &StateMachine::switchingToOnline, m_online);
    m_online->addTransition(m_online, &QState::finished, m_offline);

    m_online->setInitialState(m_connecting);
    m_connected->setInitialState(m_responding);

    connectTrace();
}

QState* StateMachine::offline() const
{
    return m_offline;
}

QState* StateMachine::online() const
{
    return m_online;
}

QFinalState* StateMachine::onlineFinal() const
{
    return m_onlineFinal;
}

QState* StateMachine::connecting() const
{
    return m_connecting;
}

QState* StateMachine::connected() const
{
    return m_connected;
}

QState* StateMachine::disconnecting() const
{
    return m_disconnecting;
}

QState* StateMachine::responding() const
{
    return m_responding;
}

QState* StateMachine::notResponding() const
{
    return m_notResponding;
}

QState* StateMachine::delayConnecting() const
{
    return m_delayConnecting;
}

QState* StateMachine::reconnecting() const
{
    return m_reconnecting;
}

QState* StateMachine::aborting() const
{
    return m_aborting;
}

QState* StateMachine::connectionFailed() const
{
    return m_connectionFailed;
}

void StateMachine::switchToOffline()
{
    emit switchingToOffline();
}

void StateMachine::switchToReconnecting()
{
    emit switchingToReconnecting();
}

void StateMachine::switchToOnline()
{
    emit switchingToOnline();
}

void StateMachine::addSwitchToOfflineTransition(QState* source, QAbstractState* target) const
{
    source->addTransition(this, &StateMachine::switchingToOffline, target);
}

void StateMachine::addSwitchToReconnectingTransition(QState* source, QAbstractState* target) const
{
    source->addTransition(this, &StateMachine::switchingToReconnecting, target);
}

void StateMachine::connectTrace()
{
    connectTrace(m_offline, QStringLiteral("offline"));
    connectTrace(m_online, QStringLiteral("online"));
    connectTrace(m_onlineFinal, QStringLiteral("onlineFinal"));

    connectTrace(m_connecting, QStringLiteral("connecting"));
    connectTrace(m_connected, QStringLiteral("connected"));
    connectTrace(m_disconnecting, QStringLiteral("disconnecting"));

    // Annoying messages
    // connectTrace(m_responding, QStringLiteral("responding"));
    connectTrace(m_notResponding, QStringLiteral("notResponding"));

    connectTrace(m_delayConnecting, QStringLiteral("delayConnecting"));
    connectTrace(m_reconnecting, QStringLiteral("reconnecting"));
    connectTrace(m_aborting, QStringLiteral("aborting"));

    connectTrace(m_connectionFailed, QStringLiteral("connectionFailed"));
}

void StateMachine::connectTrace(QAbstractState* state, const QString& name)
{
    connect(state, &QState::entered, [=]() { LOG_TRACE << "LinkStateMachine entered " << name; });
    connect(state, &QState::exited, [=]() { LOG_TRACE << "LinkStateMachine exited  " << name; });
}

} // namespace mpk::stable_link
