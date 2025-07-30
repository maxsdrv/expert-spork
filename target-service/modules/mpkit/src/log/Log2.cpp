#include "log/Log2.h"

#include "ChannelFilterFactory.h"
#include "CustomSyslogFactory.h"
#include "TimeStampFormatterFactory.h"

#include "gsl/assert"

#include <boost/filesystem.hpp>
#include <boost/log/attributes/current_process_name.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/filter_parser.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>
#include <boost/log/utility/setup/from_settings.hpp>
#include <boost/log/utility/setup/from_stream.hpp>

#include <fstream>

#ifdef __linux
#include <pwd.h>
#include <unistd.h>
#endif

namespace appkit::logger
{

namespace
{

void registerFactories()
{
    boost::log::register_sink_factory("CustomSyslog", boost::make_shared<CustomSyslogFactory>());

    boost::log::register_formatter_factory(
        "TimeStamp", boost::make_shared<TimeStampFormatterFactory>());
    boost::log::register_simple_formatter_factory<logger::LogParams, char>("Params");

    // We have some kind of magic here. On MSVS platform the order of this call
    // is essential. If we register formatter before filter, we have no message
    // in output
    boost::log::register_simple_filter_factory<logger::SeverityLevel, char>("Severity");
    boost::log::register_simple_formatter_factory<logger::SeverityLevel, char>("Severity");

    boost::log::register_filter_factory(
        "Channel", boost::make_shared<ChannelFilterFactory>(DEFAULT_CHANNEL_NAME));
}

void setupAttributes()
{
    auto core = boost::log::core::get();
    Expects(core);

    boost::log::add_common_attributes();
#ifdef __linux
    uid_t uid(geteuid());

    core->add_global_attribute("pid", boost::log::attributes::constant(getpid()));
    core->add_global_attribute("ppid", boost::log::attributes::constant(getppid()));
    core->add_global_attribute("uid", boost::log::attributes::constant(uid));
    core->add_global_attribute("gid", boost::log::attributes::constant(getgid()));
    core->add_global_attribute("euid", boost::log::attributes::constant(geteuid()));
    if (struct passwd* pw = getpwuid(uid))
    {
        core->add_global_attribute(
            "user", boost::log::attributes::constant(std::string(pw->pw_name)));
    }
#endif
    core->add_global_attribute("AppName", boost::log::attributes::current_process_name());
}

} // namespace

namespace logging = boost::log;

Log2::Log2(const std::string& configFilename)
{
    registerFactories();
    std::ifstream logSettings(configFilename.c_str());
    boost::log::init_from_stream(logSettings);
    setupAttributes();
}

Log2::Log2(const boost::log::settings& settings)
{
    registerFactories();
    logging::init_from_settings(settings);
    setupAttributes();
}

Log2::~Log2()
{
    boost::log::core::get()->remove_all_sinks();
}

} // namespace appkit::logger

BOOST_LOG_GLOBAL_LOGGER_DEFAULT(APPKIT_LOGGER, unused_param)
