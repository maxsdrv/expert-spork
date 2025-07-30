/** @file
 * @brief Application return codes
 *
 * @ingroup
 *
 *
 * $Id: $
 */

#pragma once

namespace appkit
{

enum ReturnCode
{
    SUCCESS = 0,
    RUN_TIME_ERROR,
    LOGGER_ERROR,
    INITIALIZATION_ERROR,
    CONFIGURATION_ERROR,
    DUPLICATE_RUN_ERROR
};

} // namespace appkit
