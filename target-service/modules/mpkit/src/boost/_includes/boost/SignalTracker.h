/** @file
 * @brief  Class for track
 *
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace boostsignals
{

/**
 * Use this class for tracking boost signals connection
 */
class SignalTracker
{
    /**
     * Empty class for tracking
     */
    class Void
    {
    };

public:
    SignalTracker() : m_tracker(new Void)
    {
    }

    /**
     * @return tracker object
     */
    boost::weak_ptr<void> operator()() const
    {
        return m_tracker;
    }

    /**
     * Destroy tracker
     */
    void reset()
    {
        m_tracker.reset(new Void);
    }

private:
    boost::shared_ptr<Void> m_tracker;
};

} // namespace boostsignals
