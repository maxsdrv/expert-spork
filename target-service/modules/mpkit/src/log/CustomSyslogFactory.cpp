/** @file
 * @brief
 *
 * @ingroup
 *
 * $Id: $
 */

#include "CustomSyslogFactory.h"

#include "log/SeverityLevel.h"

#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/syslog_backend.hpp>
#include <boost/log/utility/setup/filter_parser.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>

namespace appkit
{

namespace logger
{

boost::shared_ptr<boost::log::sinks::sink> CustomSyslogFactory::create_sink(
    const settings_section& settings)
{
    using namespace boost::log;

    auto sink = boost::make_shared<sinks::synchronous_sink<sinks::syslog_backend>>(
        keywords::facility = sinks::syslog::user);

    if (boost::optional<std::string> param = settings["Filter"])
    {
        sink->set_filter(boost::log::parse_filter(param.get()));
    }
    if (boost::optional<std::string> param = settings["Format"])
    {
        sink->set_formatter(boost::log::parse_formatter(param.get()));
    }
    if (boost::optional<std::string> param = settings["TargetAddress"])
    {
        sink->locked_backend()->set_target_address(param.get());
    }

    // Create and fill in another level translator for "Severity" attribute of
    // type string
    sinks::syslog::custom_severity_mapping<SeverityLevel> mapping("Severity");
    mapping[SeverityLevel::Debug] = sinks::syslog::debug;
    mapping[SeverityLevel::Info] = sinks::syslog::info;
    mapping[SeverityLevel::Warning] = sinks::syslog::warning;
    mapping[SeverityLevel::Error] = sinks::syslog::error;
    mapping[SeverityLevel::Fatal] = sinks::syslog::critical;
    sink->locked_backend()->set_severity_mapper(mapping);

    return sink;
}

} // namespace logger

} // namespace appkit
