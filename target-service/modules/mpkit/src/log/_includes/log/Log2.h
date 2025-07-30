
#pragma once

#include "mpklog_export.h"

#include "SeverityLevel.h"

#ifndef Q_MOC_RUN // Protect from BOOST_JOIN error

#include "LogParams.h"

#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/utility/setup/settings.hpp>

#endif

namespace appkit::logger
{

class MPKLOG_EXPORT Log2 : public boost::noncopyable
{
public:
    explicit Log2(const std::string& configFilename);
    explicit Log2(const boost::log::settings& settings);

    ~Log2();
};

} // namespace appkit::logger

using LoggerType = boost::log::sources::severity_channel_logger_mt<appkit::logger::SeverityLevel>;

struct MPKLOG_EXPORT APPKIT_LOGGER // NOLINT
{
    using logger_type = LoggerType; // NOLINT (readability-identifier-naming)
    enum registration_line_t
    {
        registration_line = __LINE__
    };
    static const char* registration_file() // NOLINT (readability-identifier-naming)
    {
        return __FILE__;
    }
    static logger_type construct_logger(); // NOLINT (readability-identifier-naming)
    static inline logger_type& get()
    {
        return ::boost::log::sources::aux::logger_singleton<APPKIT_LOGGER>::get();
    }
};

// Define logger with location and channel
#define LOG_WITH_LOCATION_AND_CHANNEL(LOGGER, LEVEL, CHANNEL)                                 \
    BOOST_LOG_STREAM_WITH_PARAMS(                                                             \
        (LOGGER),                                                                             \
        (::boost::log::keywords::severity =                                                   \
             (LEVEL))(::boost::log::keywords::channel = (CHANNEL)))                           \
        << boost::log::add_value("Line", __LINE__) << boost::log::add_value("File", __FILE__) \
        << boost::log::add_value("Function", __FUNCTION__)

#define DEFAULT_CHANNEL_NAME ("__default")
#define LOG_WITH_LOCATION(LOGGER, LEVEL) \
    LOG_WITH_LOCATION_AND_CHANNEL(LOGGER, LEVEL, DEFAULT_CHANNEL_NAME)

#define LOG_TRACE LOG_WITH_LOCATION(APPKIT_LOGGER::get(), appkit::logger::SeverityLevel::Trace)
#define LOG_DEBUG LOG_WITH_LOCATION(APPKIT_LOGGER::get(), appkit::logger::SeverityLevel::Debug)
#define LOG_INFO LOG_WITH_LOCATION(APPKIT_LOGGER::get(), appkit::logger::SeverityLevel::Info)
#define LOG_WARNING \
    LOG_WITH_LOCATION(APPKIT_LOGGER::get(), appkit::logger::SeverityLevel::Warning)
#define LOG_ERROR LOG_WITH_LOCATION(APPKIT_LOGGER::get(), appkit::logger::SeverityLevel::Error)
#define LOG_FATAL LOG_WITH_LOCATION(APPKIT_LOGGER::get(), appkit::logger::SeverityLevel::Fatal)

#define LOG_TRACE_CH(CHANNEL)      \
    LOG_WITH_LOCATION_AND_CHANNEL( \
        APPKIT_LOGGER::get(), appkit::logger::SeverityLevel::Trace, CHANNEL)
#define LOG_DEBUG_CH(CHANNEL)      \
    LOG_WITH_LOCATION_AND_CHANNEL( \
        APPKIT_LOGGER::get(), appkit::logger::SeverityLevel::Debug, CHANNEL)
#define LOG_INFO_CH(CHANNEL)       \
    LOG_WITH_LOCATION_AND_CHANNEL( \
        APPKIT_LOGGER::get(), appkit::logger::SeverityLevel::Info, CHANNEL)
#define LOG_WARNING_CH(CHANNEL)    \
    LOG_WITH_LOCATION_AND_CHANNEL( \
        APPKIT_LOGGER::get(), appkit::logger::SeverityLevel::Warning, CHANNEL)
#define LOG_ERROR_CH(CHANNEL)      \
    LOG_WITH_LOCATION_AND_CHANNEL( \
        APPKIT_LOGGER::get(), appkit::logger::SeverityLevel::Error, CHANNEL)
#define LOG_FATAL_CH(CHANNEL)      \
    LOG_WITH_LOCATION_AND_CHANNEL( \
        APPKIT_LOGGER::get(), appkit::logger::SeverityLevel::Fatal, CHANNEL)

// NOLINTNEXTLINE (bugprone-macro-parentheses)
#define PARAMS(PARAMS) boost::log::add_value("Params", appkit::logger::LogParams() PARAMS)
