#pragma once

#include <mutex>
#include <optional>
#include <queue>

namespace mpk::dss::core
{

struct EmptyLocker
{
    void lock(){};
    void unlock(){};
};

template <class ValueT, class BasicLockable = EmptyLocker>
class LockableQueue
{
public:
    [[nodiscard]] bool isEmpty() const
    {
        auto lock = std::scoped_lock(m_lockable);
        return m_values.empty();
    }

    [[nodiscard]] size_t size() const
    {
        auto lock = std::scoped_lock(m_lockable);
        return m_values.size();
    }

    void push(const ValueT& value)
    {
        auto lock = std::scoped_lock(m_lockable);
        m_values.push(value);
    }

    void pop()
    {
        auto lock = std::scoped_lock(m_lockable);
        m_values.pop();
    }

    [[nodiscard]] std::optional<ValueT> tryPop()
    {
        auto lock = std::scoped_lock(m_lockable);
        if (!m_values.empty())
        {
            auto command = m_values.front();
            m_values.pop();
            return command;
        }

        return {};
    }

    void clear()
    {
        auto lock = std::scoped_lock(m_lockable);
        m_values = {};
    }

private:
    std::queue<ValueT> m_values;
    mutable BasicLockable m_lockable;
};

} // namespace mpk::dss::core
