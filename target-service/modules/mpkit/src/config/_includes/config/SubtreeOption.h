/** @file
 * @brief Subtree option
 *
 * @ingroup
 *
 *
 * $Id: $
 */

#pragma once

#include "mpkconfig_export.h"

#include "Option.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/scoped_ptr.hpp>

namespace appkit
{

namespace config
{

class ConfigParser;

/**
 * Subtree option. Read any subtree from ptree
 */
class MPKCONFIG_EXPORT SubtreeOption : public Option
{
    friend class ConfigParser;

public:
    /**
     * Get description string
     */
    std::string description(int indent) const override;

protected:
    /**
     * Create option with default parameter
     */
    SubtreeOption(
        const std::string& key, const std::string& description, const ConfigParser& parser);

    /**
     * Implementation of base read method
     */
    void read(const boost::property_tree::ptree& node) override;

    /**
     * Get own key and all child keys
     */
    std::set<std::string> keys() const override;

private:
    boost::scoped_ptr<ConfigParser> m_parser;
};

} // namespace config

} // namespace appkit
