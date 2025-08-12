#pragma once

#include <cstdint>

namespace mpk::signal_scanner::detector
{

class IntervalInt
{
public:
    IntervalInt() = default;
    IntervalInt(int32_t begin, int32_t end) : m_begin(begin), m_end(end){};
    virtual ~IntervalInt() = default;

    virtual void clear()
    {
        m_begin = 0;
        m_end = 0;
    };

    [[nodiscard]] bool inInterval(int32_t value) const
    {
        return value >= m_begin && value <= m_end;
    }
    [[nodiscard]] int32_t center() const
    {
        return (m_begin + m_end) / 2;
    }

    [[nodiscard]] int32_t begin() const
    {
        return m_begin;
    }
    void setBegin(int32_t begin)
    {
        m_begin = begin;
    }
    [[nodiscard]] int32_t end() const
    {
        return m_end;
    }
    void setEnd(int32_t end)
    {
        m_end = end;
    }
    [[nodiscard]] int32_t size() const
    {
        return m_end - m_begin;
    }

private:
    int32_t m_begin = 0;
    int32_t m_end = 0;
};

} // namespace mpk::signal_scanner::detector