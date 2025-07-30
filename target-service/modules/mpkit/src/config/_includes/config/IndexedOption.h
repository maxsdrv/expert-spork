/** @file
 * @brief Option with indexed placeholder
 *
 * @ingroup
 *
 *
 * $Id: $
 */

#pragma once

#include "Option.h"

#include "boost/translations/Translations.h"
#include "exception/InvalidConfigEntry.h"
#include "exception/KeyInfo.h"
#include "types/Macro.h"

#include "gsl/assert"

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
class IndexedOption : public Option
{
    friend class ConfigParser;

public:
    /**
     * Option default value setter
     */
    IndexedOption<T>& setDefaultValue(const T& value)
    {
        m_defaultValue = boost::optional<T>(value);
        return *this;
    }

    /**
     * Set max index for parsing
     */
    IndexedOption<T>& setMaxIndex(int maxIndex)
    {
        m_maxIndex = maxIndex;
        return *this;
    }

    /**
     * Set placeholder for option
     */
    IndexedOption<T>& setPlaceholder(char placeholder)
    {
        updatePaths(placeholder);
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
                TR_N("APPKIT", "indexed, default value = {1}", m_defaultValue) :
                TR_N("APPKIT", "indexed, mandatory");

        return TR_N("APPKIT", "{1} ({2})", descriptionValue(), defaultValue);
    }

    /**
     * Get own key and all child keys
     */
    std::set<std::string> keys() const override
    {
        std::set<std::string> allKeys;
        for (int i = 0; i <= m_maxIndex; ++i)
        {
            allKeys.insert(
                (boost::format("%1$s%2$d.%3$s") % m_parentPath % i % m_childPath).str());
        }

        return allKeys;
    }

private:
    /**
     * Create mandatory indexed option+
     */
    IndexedOption(
        boost::function<void(int index, T&)> setter,
        const std::string& key,
        const std::string& description) :
      Option(key, description), m_setter(setter)
    {
        updatePaths('%'); // default placeholder
    }

    void updatePaths(char placeholder)
    {
        const std::string k(key());
        size_t placeholderPosition(k.find(placeholder));
        if (placeholderPosition != std::string::npos)
        {
            m_parentPath = k.substr(0, placeholderPosition);
            m_childPath = k.substr(placeholderPosition + 2); // 2 = placeholder + dot
        }
    }

    /**
     * Implementation of base read method
     */
    void read(const boost::property_tree::ptree& node) override
    {
        Expects(m_parentPath != key());

        // Say, we have option.%.node here
        // We check while nodes option.0, option.1 etc are presented in tree
        // then read with exception or read with default (like in ValueOption)
        for (int idx = 0; idx < m_maxIndex; ++idx)
        {
            std::string subKey = (boost::format("%1$s%2$d") % m_parentPath % idx).str();

            auto child = node.get_child_optional(subKey);
            // No childs here, return
            if (!child)
            {
                return;
            }

            try
            {
                if (m_defaultValue)
                {
                    T value = child->get<T>(m_childPath, m_defaultValue.get());
                    m_setter(idx, value);
                }
                else
                {
                    T value = child->get<T>(m_childPath);
                    m_setter(idx, value);
                }
            }
            catch (const boost::property_tree::ptree_bad_data& ex)
            {
                std::vector<std::string> keysInfo(
                    1, (boost::format("%1$s.%2$s") % subKey % m_childPath).str());
                keysInfo.push_back(ex.data<std::string>());
                BOOST_THROW_EXCEPTION(
                    exception::InvalidConfigEntry()
                    << exception::KeysInfo(keysInfo) << exception::toForeignExceptionInfo(ex));
            }
        }
    }

private:
    boost::function<void(int index, T&)> m_setter;
    boost::optional<T> m_defaultValue;
    std::string m_parentPath;
    std::string m_childPath;
    static constexpr auto MAX_INDEX_DEFAULT = 9;
    int m_maxIndex = MAX_INDEX_DEFAULT;
};

} // namespace config

} // namespace appkit
