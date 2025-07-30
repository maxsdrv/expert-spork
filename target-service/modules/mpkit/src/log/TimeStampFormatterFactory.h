/** @file
 * @brief Timestamp formatter factory
 *
 * @ingroup
 *
 * $Id: $
 */

#pragma once

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/expressions/formatters/date_time.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>

namespace appkit
{

namespace logger
{

/**
 * Custom formatter factory to add TimeStamp format support in config ini file.
 * Allows %TimeStamp(format=\"%Y.%m.%d %H:%M:%S.%f\")% to be used in ini config
 * file for property Format.
 */
class TimeStampFormatterFactory
  : public boost::log::basic_formatter_factory<char, boost::posix_time::ptime>
{
public:
    /**
     * The function creates a formatter for the specified attribute.
     */
    formatter_type create_formatter(
        const boost::log::attribute_name& name, const args_map& args) final
    {
        auto it = args.find("format");
        if (it != args.end())
        {
            return boost::log::expressions::stream
                   << boost::log::expressions::format_date_time<boost::posix_time::ptime>(
                          boost::log::expressions::attr<boost::posix_time::ptime>(name),
                          it->second);
        }

        return boost::log::expressions::stream
               << boost::log::expressions::attr<boost::posix_time::ptime>(name);
    }
};

} // namespace logger

} // namespace appkit
