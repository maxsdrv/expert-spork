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

#include "types/Macro.h"

#include "boost/translations/Translations.h"

#include <boost/format.hpp>
#include <boost/function.hpp>
#include <boost/property_tree/ptree.hpp>

#include <iostream>

namespace appkit
{

namespace config
{

/**
 * Parametrized option. Can read array option from ptree
 */
template <class T>
class ArrayOption : public Option
{
    friend class ConfigParser;

public:
    /**
     * Get description string
     */
    std::string description(int indent) const override
    {
        UNUSED(indent);
        return TR_N("APPKIT", "{1} (array)", descriptionValue());
    }

private:
    /**
     * Create array option
     */
    ArrayOption(
        boost::function<void(T&)> setter,
        const std::string& key,
        const std::string& description) :
      Option(key, description), m_setter(setter)
    {
    }

    /**
     * Implementation of base read method
     */
    void read(const boost::property_tree::ptree& node) override
    {
        try
        {
            for (const auto& v: node.get_child(key()))
            {
                T value = v.second.template get<T>("");
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

    /**
     * Get own key and all child keys
     */
    std::set<std::string> keys() const override
    {
        return {key()};
    }

private:
    boost::function<void(T&)> m_setter;
};

} // namespace config

} // namespace appkit
