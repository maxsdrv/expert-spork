/**
 * @file
 * @brief Helper macro for adding boost signals to class
 *
 *
 * $Id: $
 **/

#pragma once

#include <boost/signals2.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/weak_ptr.hpp>

namespace boostsignals
{

template <class SignalType>
class ConnectHelper
{
public:
    explicit ConnectHelper(SignalType& signal) : m_signal(signal)
    {
    }

public:
    boost::signals2::connection connect(const typename SignalType::slot_type& subscriber)
    {
        return m_signal.connect(subscriber);
    }

private:
    SignalType& m_signal;
};

/**
 * Add new signal-driven task with message
 */
template <class SignalType>
void singleShot(ConnectHelper<SignalType> helper, const typename SignalType::slot_type& subscriber)
{
    BOOST_AUTO(connection, helper.connect(subscriber));
    helper.connect(boost::bind(&boost::signals2::connection::disconnect, connection));
}

} // namespace boostsignals

// Add signal helper
#define ADD_SIGNAL_HELPER(NAME, SIGNATURE, TYPENAME)                                             \
public:                                                                                          \
    typedef boost::signals2::signal<SIGNATURE> NAME##Signal;                                     \
                                                                                                 \
protected:                                                                                       \
    mutable NAME##Signal m_##NAME; /*NOLINT*/                                                    \
                                                                                                 \
public:                                                                                          \
    boost::signals2::connection on##NAME(                                                        \
        TYPENAME NAME##Signal::slot_type subscriber, const boost::weak_ptr<void>& trackedObject) \
        const                                                                                    \
    {                                                                                            \
        subscriber.track(trackedObject);                                                         \
        return m_##NAME.connect(subscriber);                                                     \
    }                                                                                            \
    boostsignals::ConnectHelper<NAME##Signal> NAME##Helper() const                               \
    {                                                                                            \
        return boostsignals::ConnectHelper<NAME##Signal>(m_##NAME);                              \
    }

#define ADD_SIGNAL(NAME, SIGNATURE) ADD_SIGNAL_HELPER(NAME, SIGNATURE, )
#define ADD_SIGNAL_T(NAME, SIGNATURE) ADD_SIGNAL_HELPER(NAME, SIGNATURE, typename)
