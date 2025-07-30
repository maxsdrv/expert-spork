#pragma once

#include "General.h"

namespace exception
{

template <typename String>
using StringValue = boost::error_info<struct stringValue_, String>;

template <typename String>
class StringParseFailed : public virtual General
{
public:
    explicit StringParseFailed(const String& value)
    {
        (*this) << StringValue<String>(value);
    }
};

} // namespace exception
