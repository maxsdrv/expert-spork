/** @file
 * @brief Helper class for adding Params attribute to logger
 *
 * @ingroup
 *
 *
 * $Id: $
 */

#pragma once

#include "mpklog_export.h"

#include <boost/lexical_cast.hpp>

#include <string>
#include <vector>

namespace appkit::logger
{

/**
 * This class helps add Params to logger
 */
class MPKLOG_EXPORT LogParams
{
    friend std::ostream& operator<<(std::ostream& stream, const LogParams& level);

public:
    /**
     * Add single parameter to params
     */
    template <typename T>
    LogParams& operator()(const T& value)
    {
        m_params.push_back(boost::lexical_cast<std::string>(value));
        return *this;
    }

    /**
     * Add single named parameter to params
     */
    template <typename T>
    LogParams& operator()(const std::string& name, const T& value)
    {
        m_params.push_back(name + "=" + boost::lexical_cast<std::string>(value));
        return *this;
    }

private:
    std::vector<std::string> m_params;
};

// To stream operator. Required for formatter
std::ostream& operator<<(std::ostream& stream, const LogParams& level) MPKLOG_EXPORT;

} // namespace appkit::logger
