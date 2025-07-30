/** @file
 * @brief     Factory for creating specific StateConverter
 *
 * $Id: $
 */

#pragma once

#include "converters/ActionStateConverter.h"
#include "converters/HeaderViewStateConverter.h"
#include "converters/SplitterStateConverter.h"
#include "converters/WidgetStateConverter.h"

#include <boost/variant.hpp>

#include <map>

namespace uistatesaver
{

using ConverterTraits = boost::variant<ActionTraits, SplitterTraits, HeaderTraits, WidgetTraits>;
using ConverterTraitsMap = std::vector<std::pair<std::string, ConverterTraits>>;

// Note: widget must be the last one
const ConverterTraitsMap AVAILABLE_TRAITS = {
    {"action", ActionTraits()},
    {"headerview", HeaderTraits()},
    {"splitter", SplitterTraits()},
    {"widget", WidgetTraits()}};

/**
 * Function for setting property id
 */
template <typename T>
QString statePropertyKey(T* object)
{
    return QString("%1_STATE_SAVE_ID").arg(object->metaObject()->className());
}

/**
 * Function for setting property id
 */
template <typename T>
void setStatePropertyId(T* object, const QString& id)
{
    object->setProperty(statePropertyKey(object).toLatin1().constData(), id);
}

} // namespace uistatesaver
