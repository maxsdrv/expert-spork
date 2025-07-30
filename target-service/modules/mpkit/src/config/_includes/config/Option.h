/** @file
 * @brief
 *
 * @ingroup
 *
 *
 * $Id: $
 */

#pragma once

#include <boost/property_tree/ptree_fwd.hpp>

#include <set>
#include <string>

namespace appkit
{

namespace config
{

/**
 * Base class for options
 */
class Option
{
public:
    Option(std::string key, std::string description) :
      m_key(std::move(key)), m_description(std::move(description))
    {
    }
    /**
     * Virtual destructor
     */
    virtual ~Option() = default;

    /**
     * Read specified option from @a config
     */
    virtual void read(const boost::property_tree::ptree& config) = 0;

    /**
     * Get description string
     */
    virtual std::string description(int indent) const = 0;

    /**
     * Get own key and all child keys
     */
    virtual std::set<std::string> keys() const = 0;

    /**
     * Return own key
     */
    std::string key() const
    {
        return m_key;
    }

protected:
    /**
     * Get description value
     */
    std::string descriptionValue() const
    {
        return m_description;
    }

private:
    std::string m_key;
    std::string m_description;
};

} // namespace config

} // namespace appkit
