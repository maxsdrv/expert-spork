#pragma once

namespace mpk
{

namespace storage_adapter
{

// Typical usage for this object is adapting databases, but one can
// use it in any appropriate context
template <typename ValueType>
class ProxyObject
{
public:
    // read interface
    void update();
    const ValueType& value(int index) const;
    int size() const;

    // write interface
    void append(const ValueType& value);
    void replace(int index, const ValueType& value);
    void remove(int index);
    void clear();
};

} // namespace storage_adapter

} // namespace mpk
