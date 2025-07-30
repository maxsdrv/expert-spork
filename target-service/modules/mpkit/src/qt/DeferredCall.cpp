#include "qt/DeferredCall.h"

#include <QTimer>

namespace qt
{

DeferredCall::DeferredCall(Call call, int timeout, QObject* parent) :
  QObject(parent),
  m_call(std::move(call)),
  m_guarded(std::make_shared<bool>(false)),
  m_guard(m_guarded),
  m_timer(new QTimer(this))
{
    m_timer->setInterval(timeout);
    connect(m_timer, &QTimer::timeout, this, &DeferredCall::trigger);
}

DeferredCall::DeferredCall(Call call, std::weak_ptr<bool> guard, int timeout, QObject* parent) :
  QObject(parent), m_call(std::move(call)), m_guard(std::move(guard)), m_timer(new QTimer(this))
{
    m_timer->setInterval(timeout);
    connect(m_timer, &QTimer::timeout, this, &DeferredCall::trigger);
}

void DeferredCall::run()
{
    m_timer->start();
}

void DeferredCall::runFree()
{
    m_timer->start();
    m_suicide = true;
}

void DeferredCall::asyncRun(Call call)
{
    auto* deferred = new DeferredCall(std::move(call), 0);
    deferred->runFree();
}

void DeferredCall::asyncRun(Call call, std::weak_ptr<bool> guard)
{
    auto* deferred = new DeferredCall(std::move(call), std::move(guard), 0);
    deferred->runFree();
}

void DeferredCall::trigger()
{
    if (!m_guard.expired())
    {
        m_call();
    }
    m_timer->stop();
    if (m_suicide)
    {
        deleteLater();
    }
}

} // namespace qt
