/** @file
 * @brief      Output to Boost.Log comma-separated values from std::vector.
 *
 * $Id: $
 */

#pragma once

#include "ContainerPrinter.h"

#include <vector>

namespace std
{

inline std::ostream& operator<<(std::ostream& strm, const std::vector<int>& container)
{
    return print(strm, container, "vector");
}

} // namespace std
