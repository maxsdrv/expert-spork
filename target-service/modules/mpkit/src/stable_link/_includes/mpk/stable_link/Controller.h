#pragma once

#include "mpk/stable_link/Link.h"
#include "mpk/stable_link/Policy.h"
#include "mpk/stable_link/StateMachine.h"

#include <QTimer>

#include <memory>

namespace mpk::stable_link
{

class Controller : public QObject
{
    Q_OBJECT

public:
    using LinkSignalsPin = Link::LinkSignalsPin;

    explicit Controller(std::unique_ptr<Policy> linkPolicy, QObject* parent = nullptr);

    void setPolicy(std::unique_ptr<Policy> linkPolicy);

    void switchToOnline();
    void switchToOffline();

    State linkState() const;
    LinkSignalsPin linkSignals() const;

    // Connect external link signals to this signals.
signals:
    void linkConnected();
    void linkDisconnected();
    void linkResponded();
    void linkInterrupted();

    // Connect external link methods to this signals.
signals:
    void connectLink();
    void disconnectLink();
    void abortLink();

    // Ordinary signals.
signals:
    void connecting();
    void connected();
    void disconnected();

private:
    void setupLinkStateTransitions();
    void connectToLinkState();
    void notifyLinkState(State linkState);
    std::string logName() const;

private:
    State m_linkState{State::OFFLINE};
    Signals* m_linkSignals;
    StateMachine* m_stateMachine;
    QTimer* m_timer;
    std::unique_ptr<Policy> m_linkPolicy;
};

} // namespace mpk::stable_link
