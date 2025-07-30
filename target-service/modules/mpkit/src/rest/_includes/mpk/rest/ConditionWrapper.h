#pragma once

#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>

#include <optional>

namespace mpk::rest
{

template <class ObjectType>
class ConditionWrapper
{
private:
    QMutex m_mutex;

public:
#if QT_MAJOR < 6
    using Lock = QMutexLocker;
#else
    using Lock = QMutexLocker<decltype(m_mutex)>;
#endif
    Lock lock()
    {
        return QMutexLocker{&m_mutex};
    }

    void setResult(const ObjectType& result)
    {
        auto localLock = lock();
        m_resultOpt = result;
        notify();
    }

    ObjectType waitResult()
    {
        auto localLock = lock();

        if (!m_resultOpt.has_value())
        {
            m_conditionVariable.wait(&m_mutex);
        }

        ObjectType result = *m_resultOpt;
        m_resultOpt.reset();
        return result;
    }

    void notify()
    {
        m_conditionVariable.notify_all();
    }

private:
    QWaitCondition m_conditionVariable;
    std::optional<ObjectType> m_resultOpt;
};

} // namespace mpk::rest
