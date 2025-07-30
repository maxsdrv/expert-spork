#pragma once

namespace mpk::dss::core
{

class QueryCursor;

template <typename T>
T fromQueryCursor(QueryCursor* cursor);

} // namespace mpk::dss::core
