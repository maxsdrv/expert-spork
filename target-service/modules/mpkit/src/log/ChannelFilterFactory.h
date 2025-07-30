#pragma once

#include <boost/log/utility/setup/filter_parser.hpp>

#include <string>

namespace appkit
{

namespace logger
{

struct ChannelFilterFactory : public boost::log::filter_factory<char>
{
    explicit ChannelFilterFactory(std::string defaultChannel);

    boost::log::filter on_exists_test(boost::log::attribute_name const& name) override;

    boost::log::filter on_equality_relation(
        boost::log::attribute_name const& name, string_type const& arg) override;

    boost::log::filter on_inequality_relation(
        boost::log::attribute_name const& name, string_type const& arg) override;

    boost::log::filter on_custom_relation(
        boost::log::attribute_name const& name,
        string_type const& rel,
        string_type const& arg) override;

    const std::string m_defaultChannel;
};

} // namespace logger

} // namespace appkit
