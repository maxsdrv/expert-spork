#include "mpk/rest/HttpUtility.h"

namespace mpk::rest
{

OperationResult::Value toOperationResult(http::HttpCode code)
{
    using http::HttpCode;
    switch (code)
    {
        case HttpCode::Ok: return OperationResult::SUCCESS;
        case HttpCode::Unauthorized: return OperationResult::UNAUTHORIZED;
        case HttpCode::Forbidden: return OperationResult::NOT_PERMITTED;
        default: return OperationResult::UNKNOWN;
    }
}

} // namespace mpk::rest
