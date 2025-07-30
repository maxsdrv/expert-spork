#pragma once

#include <QByteArray>

#include <vector>

namespace rpc
{

class NullMessageWrapper
{
public:
    static QByteArray wrap(const QByteArray& array)
    {
        return array;
    }

    static std::vector<QByteArray> unwrap(const QByteArray& array)
    {
        return {array};
    }
};

} // namespace rpc
