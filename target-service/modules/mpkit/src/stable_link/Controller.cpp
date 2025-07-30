#include "mpk/stable_link/Controller.h"

#include "log/Log2.h"

#include <QCoreApplication>

namespace mpk::stable_link
{

Controller::Controller(std::unique_ptr<Policy> linkPolicy, QObject* parent) :
  QObject{parent},
  m_linkSignals{new Signals(this)},
  m_stateMachine{new StateMachine(this)},
  m_timer{new QTimer(this)},
  m_linkPolicy{std::move(linkPolicy)}
{
    Q_ASSERT(m_linkPolicy);

    qRegisterMetaType<State>("mpk::stable_link::State");

    m_timer->setSingleShot(true);

    setupLinkStateTransitions();
    connectToLinkState();
    m_stateMachine->start();
}

void Controller::setPolicy(std::unique_ptr<Policy> linkPolicy)
{
    Q_ASSERT(linkPolicy);
    m_linkPolicy = std::move(linkPolicy);
}

void Controller::switchToOnline()
{
    m_stateMachine->switchToOnline();
}

void Controller::switchToOffline()
{
    m_stateMachine->switchToOffline();
}

State Controller::linkState() const
{
    return m_linkState;
}

Controller::LinkSignalsPin Controller::linkSignals() const
{
    return m_linkSignals;
}

void Controller::setupLinkStateTransitions()
{
    // See the state diagram of the StableLinkState
    // in the rct/docs/core/StableLinkState.plantuml.
    // Please update the StableLinkState.plantuml according to
    // setupLinkStateTransitions() and connectToLinkState() methods.

    // Connecting
    m_stateMachine->connecting()->addTransition(
        this, &Controller::linkConnected, m_stateMachine->connected());

    m_stateMachine->connecting()->addTransition(
        this, &Controller::linkInterrupted, m_stateMachine->connectionFailed());

    m_stateMachine->connecting()->addTransition(
        m_timer, &QTimer::timeout, m_stateMachine->reconnecting());

    m_stateMachine->addSwitchToOfflineTransition(
        m_stateMachine->connecting(), m_stateMachine->aborting());

    // Connected
    m_stateMachine->connected()->addTransition(
        this, &Controller::linkInterrupted, m_stateMachine->connectionFailed());

    m_stateMachine->addSwitchToOfflineTransition(
        m_stateMachine->connected(), m_stateMachine->disconnecting());

    // Disconnecting
    m_stateMachine->disconnecting()->addTransition(
        this, &Controller::linkDisconnected, m_stateMachine->onlineFinal());

    // Responding
    m_stateMachine->responding()->addTransition(
        this, &Controller::linkResponded, m_stateMachine->responding());

    m_stateMachine->responding()->addTransition(
        m_timer, &QTimer::timeout, m_stateMachine->notResponding());

    // Not responding
    m_stateMachine->addSwitchToReconnectingTransition(
        m_stateMachine->notResponding(), m_stateMachine->reconnecting());
    m_stateMachine->addSwitchToOfflineTransition(
        m_stateMachine->notResponding(), m_stateMachine->disconnecting());

    // Delay connecting
    m_stateMachine->delayConnecting()->addTransition(
        m_timer, &QTimer::timeout, m_stateMachine->connecting());

    m_stateMachine->addSwitchToOfflineTransition(
        m_stateMachine->delayConnecting(), m_stateMachine->onlineFinal());

    // Reconnecting
    m_stateMachine->reconnecting()->addTransition(m_stateMachine->delayConnecting());

    // Aborting
    m_stateMachine->aborting()->addTransition(m_stateMachine->onlineFinal());

    // Connection failed
    m_stateMachine->connectionFailed()->addTransition(m_stateMachine->delayConnecting());
}

void Controller::connectToLinkState()
{
    // See the state diagram of the StableLinkState
    // in the rct/docs/core/StableLinkState.plantuml.
    // Please update the StableLinkState.plantuml according to
    // setupLinkStateTransitions() and connectToLinkState() methods.

    // Online
    connect(
        m_stateMachine->online(),
        &QState::entered,
        this,
        [this]() { m_linkPolicy->resetConnectingTries(); });

    // Connecting
    connect(
        m_stateMachine->connecting(),
        &QState::entered,
        this,
        [this]()
        {
            m_linkPolicy->nextConnectingTry();

            if (auto connectingTimeout = m_linkPolicy->connectingTimeout())
            {
                m_timer->start(*connectingTimeout);
            }
            notifyLinkState(State::CONNECTING);
            emit connecting();
            emit connectLink();
        });

    connect(m_stateMachine->connecting(), &QState::exited, this, [this]() { m_timer->stop(); });

    // Connected
    connect(
        m_stateMachine->connected(),
        &QState::entered,
        this,
        [this]()
        {
            m_linkPolicy->resetConnectingTries();
            notifyLinkState(State::CONNECTED);
            emit connected();
        });

    connect(
        m_stateMachine->connected(),
        &QState::exited,
        this,
        [this]()
        {
            m_timer->stop();
            notifyLinkState(State::DISCONNECTED);
            emit disconnected();
        });

    // Disconnecting
    connect(
        m_stateMachine->disconnecting(),
        &QState::entered,
        this,
        [this]() { emit disconnectLink(); });

    // Responding
    connect(
        m_stateMachine->responding(),
        &QState::entered,
        this,
        [this]()
        {
            if (auto respondingTimeout = m_linkPolicy->respondingTimeout())
            {
                m_timer->start(*respondingTimeout);
            }
        });

    // Not responding
    connect(
        m_stateMachine->notResponding(),
        &QState::entered,
        this,
        [this]()
        {
            LOG_ERROR << logName() << "Connection not responding";

            auto action = m_linkPolicy->notRespondingAction();
            switch (action)
            {
                case Policy::Action::Reconnect: m_stateMachine->switchToReconnecting(); break;
                case Policy::Action::Disconnect: m_stateMachine->switchToOffline(); break;
            }
        });

    // Delay connecting
    connect(
        m_stateMachine->delayConnecting(),
        &QState::entered,
        this,
        [this]()
        {
            if (auto connectingDelay = m_linkPolicy->connectingDelay())
            {
                const std::chrono::milliseconds delayTimeout = *connectingDelay;
                LOG_INFO << logName() << "Trying to reconnect after " << delayTimeout.count()
                         << "ms";
                m_timer->start(delayTimeout);
            }
            else
            {
                LOG_INFO << logName() << "Tries to reconnect ended";
                m_stateMachine->switchToOffline();
            }
        });

    connect(
        m_stateMachine->delayConnecting(), &QState::exited, this, [this]() { m_timer->stop(); });

    // Reconnecting
    connect(
        m_stateMachine->reconnecting(), &QState::entered, this, [this]() { emit abortLink(); });

    // Aborting
    connect(m_stateMachine->aborting(), &QState::entered, this, [this]() { emit abortLink(); });

    // Connection failed
    connect(
        m_stateMachine->connectionFailed(),
        &QState::entered,
        this,
        [this]() { LOG_ERROR << logName() << "Connection failed"; });

    // Offline
    connect(
        m_stateMachine->offline(),
        &QState::entered,
        this,
        [this]() { notifyLinkState(State::OFFLINE); });
}

void Controller::notifyLinkState(State linkState)
{
    m_linkState = linkState;
    emit m_linkSignals->linkStateChanged(m_linkState);
}

std::string Controller::logName() const
{
    return "[" + objectName().toStdString() + "]: ";
}

} // namespace mpk::stable_link
