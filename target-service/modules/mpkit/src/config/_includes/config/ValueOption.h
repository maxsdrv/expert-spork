/** @file
 * @brief
 *
 * @ingroup
 *
 *
 * $Id: $
 */

#pragma once

#include "Option.h"

#include "exception/InvalidConfigEntry.h"
#include "exception/KeyInfo.h"

#include "boost/translations/Translations.h"
#include "types/Macro.h"

#include <boost/format.hpp>
#include <boost/function.hpp>
#include <boost/property_tree/ptree.hpp>

namespace appkit
{

namespace config
{

/**
 * Parametrized option class. Can read single option from ptree
 */
template <class T>
class ValueOption : public Option
{
    friend class ConfigParser;

public:
    /**
     * Option default value setter
     */
    ValueOption<T>& setDefaultValue(const T& value)
    {
        m_defaultValue = boost::optional<T>(value);
        return *this;
    }

    /**
     * Get description string
     */
    std::string description(int indent) const override
    {
        UNUSED(indent);
        const std::string& defaultValue =
            m_defaultValue.is_initialized() ?
                TR_N("APPKIT", "default value = {1}", m_defaultValue) :
                TR_N("APPKIT", "mandatory");

        return TR_N("APPKIT", "{1} ({2})", descriptionValue(), defaultValue);
    }

    /**
     * Get own key and all child keys
     */
    std::set<std::string> keys() const override
    {
        return {key()};
    }

private:
    /**
     * Create option with default parameter
     */
    ValueOption(
        boost::function<void(T&)> setter,
        const std::string& key,
        const std::string& description) :
      Option(key, description), m_setter(std::move(setter))
    {
    }

    /**
     * Implementation of base read method
     */
    void read(const boost::property_tree::ptree& node) override
    {
        try
        {
            if (m_defaultValue)
            {
                T value = node.get<T>(key(), m_defaultValue.get());
                m_setter(value);
            }
            else
            {
                T value = node.get<T>(key());
                m_setter(value);
            }
        }
        catch (const boost::property_tree::ptree_bad_data& ex)
        {
            std::vector<std::string> keysInfo(1, key());
            keysInfo.push_back(ex.data<std::string>());
            BOOST_THROW_EXCEPTION(
                exception::InvalidConfigEntry()
                << exception::KeysInfo(keysInfo) << exception::toForeignExceptionInfo(ex));
        }
    }

private:
    boost::function<void(T&)> m_setter;
    boost::optional<T> m_defaultValue;
};

} // namespace config

} // namespace appkit
