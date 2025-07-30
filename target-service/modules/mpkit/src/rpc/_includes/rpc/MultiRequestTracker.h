#pragma once

#include "rpc/RequestTracker.h"

#include <map>

namespace rpc
{

// This class tracks several requests and emits complete signal when all
// requests are done
class MultiRequestTracker
{
    ADD_SIGNAL(Complete, void())

public:
    void track(const std::string& id, const std::shared_ptr<RequestTrackerBase>& requestTracker);

    template <typename R>
    std::shared_ptr<RequestTracker<R>> tracker(std::string const& id) const
    {
        auto it = m_trackers.find(id);
        if (it == m_trackers.end())
        {
            return {};
        }
        return std::dynamic_pointer_cast<RequestTracker<R>>(it->second);
    }

    bool succeeded() const;

private:
    void checkComplete();

private:
    std::map<std::string, std::shared_ptr<RequestTrackerBase>> m_trackers;
    std::map<std::string, bool> m_triggered;

    boostsignals::SignalTracker m_signalGuard;
};

} // namespace rpc
