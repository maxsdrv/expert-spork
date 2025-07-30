#pragma once

#include "httpws/Definitions.h"

#include "mpk/rest/OperationResult.h"

namespace mpk::rest
{

// TODO: A duplicate of the rctcore::toOperationResult,
// take it to the common library.
OperationResult::Value toOperationResult(http::HttpCode code);

} // namespace mpk::rest
