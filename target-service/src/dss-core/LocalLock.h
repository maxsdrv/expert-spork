#pragma once

#include <mutex>

namespace mpk::dss::core
{
class LocalLock
{
public:
    LocalLock() = default;
    LocalLock(const LocalLock&) = delete;

    [[nodiscard]] std::unique_lock<std::mutex> local() const
    {
        return std::unique_lock(m_mtx);
    }

private:
    mutable std::mutex m_mtx;
};
} // namespace mpk::dss::core
