#include "ChannelFilterFactory.h"

#include "BurstControl.h"

namespace appkit
{

namespace logger
{

ChannelFilterFactory::ChannelFilterFactory(std::string defaultChannel) :
  m_defaultChannel(std::move(defaultChannel))
{
}

boost::log::filter ChannelFilterFactory::on_exists_test(boost::log::attribute_name const& name)
{
    return boost::log::expressions::has_attr<std::string>(name);
}

boost::log::filter ChannelFilterFactory::on_equality_relation(
    boost::log::attribute_name const& name, string_type const& arg)
{
    auto filter = [name, arg](const auto& attrs) -> bool
    {
        auto channel_ref = boost::log::extract<std::string>(name, attrs);
        return !channel_ref || *channel_ref == arg;
    };
    return filter;
}

boost::log::filter ChannelFilterFactory::on_inequality_relation(
    boost::log::attribute_name const& name, string_type const& arg)
{
    auto filter = [name, arg](const auto& attrs) -> bool
    {
        auto channel_ref = boost::log::extract<std::string>(name, attrs);
        return !channel_ref || *channel_ref != arg;
    };
    return filter;
}

boost::log::filter ChannelFilterFactory::on_custom_relation(
    boost::log::attribute_name const& name, string_type const& rel, string_type const& arg)
{
    auto createFilter = [name, defaultChannel = m_defaultChannel](auto delay)
    {
        return [delay, name, defaultChannel](const auto& attrs) -> bool
        {
            static BurstControl burstControl(delay);
            auto channel_ref = boost::log::extract<std::string>(name, attrs);
            return !channel_ref || *channel_ref == defaultChannel
                   || burstControl.verify(*channel_ref);
        };
    };

    if (rel == "delay_s")
    {
        auto delay = std::chrono::duration_cast<BurstControl::Clock::duration>(
            std::chrono::seconds(std::stol(arg)));
        return createFilter(delay);
    }

    if (rel == "delay_ms")
    {
        auto delay = std::chrono::duration_cast<BurstControl::Clock::duration>(
            std::chrono::milliseconds(std::stol(arg)));
        return createFilter(delay);
    }

    throw std::runtime_error("Unsupported filter relation: " + rel);
}

} // namespace logger

} // namespace appkit
