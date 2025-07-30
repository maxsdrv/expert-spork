/** @file
 * @brief
 *
 * @ingroup
 *
 * $Id: $
 */

#pragma once

#include <boost/log/utility/setup/from_settings.hpp>

namespace appkit
{

namespace logger
{

/**
 * This class creates syslog sink from setting. We don't use standard factory
 * because we want to add severity mapping to syslog sink
 *
 *
We support the following parameters
    Destination - type of sink, should be CustomSyslog (mandatory)
    Filter - regular log filter pattern
    Format - regular log formatter pattern
    TargetAddress - syslog ip

    EXAMPLE:

        [Sinks.Syslog]

        Destination=CustomSyslog
        Filter="%Scope% = TestLogger and %Severity% > DEBUG"
        Format="%Scope% [%TimeStamp%]: <%Severity%> %Message%"
        TargetAddress=127.0.0.1
**/
class CustomSyslogFactory : public boost::log::sink_factory<char>
{
public:
    // Creates the sink with the provided parameters
    boost::shared_ptr<boost::log::sinks::sink> create_sink(
        const settings_section& settings) override;
};

} // namespace logger

} // namespace appkit
