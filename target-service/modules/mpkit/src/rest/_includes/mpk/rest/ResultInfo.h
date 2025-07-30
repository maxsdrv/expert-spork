#pragma once

#include "mpk/rest/ErrorInfo.h"

#include <optional>

namespace mpk::rest
{

template <class ResultType>
struct ResultInfo
{
    using ResultObject = ResultType;
    using ErrorInfoOpt = std::optional<ErrorInfo>;

    ResultObject data;
    ErrorInfoOpt error;
};

} // namespace mpk::rest
