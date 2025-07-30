#include "rpc/MultiRequestTracker.h"

namespace rpc
{

void MultiRequestTracker::track(
    std::string const& id, std::shared_ptr<RequestTrackerBase> const& requestTracker)
{
    m_trackers.insert({id, requestTracker});
    m_triggered.insert({id, false});
    // Clang static analyzer fires false positive
    // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDelete)
    requestTracker->onComplete(
        [id, this]()
        {
            m_triggered[id] = true;
            checkComplete();
        },
        m_signalGuard());
}

bool MultiRequestTracker::succeeded() const
{
    return std::all_of(
        m_trackers.begin(),
        m_trackers.end(),
        [](const auto& tracker) { return tracker.second->succeeded(); });
}

void MultiRequestTracker::checkComplete()
{
    if (std::all_of(
            m_triggered.begin(),
            m_triggered.end(),
            [](auto const& triggered) { return triggered.second; }))
    {
        m_Complete();
    }
}

} // namespace rpc
