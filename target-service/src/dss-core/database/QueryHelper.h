#pragma once

#include <QString>

#include <memory>
#include <vector>

namespace mpk::dss::core
{

class QueryCursor;

template <typename T, typename U = T>
class QueryHelper
{
public:
    std::vector<T> fetchData();
    std::unique_ptr<QueryCursor> data();
    void append(const T& value);
    void replace(const QString& key, const T& value);
    void remove(const QString& key);
    void clear();
};

} // namespace mpk::dss::core
