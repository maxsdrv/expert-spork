/** @file
 * @brief      Output to Boost.Log comma-separated values from container.
 *
 * $Id: $
 */

#pragma once

#include <boost/format.hpp>
#include <boost/log/utility/formatting_ostream.hpp>
#include <utility>

template <class First, class Second, class charT, class traits>
std::basic_ostream<charT, traits>& operator<<(
    std::basic_ostream<charT, traits>& stream, const std::pair<First, Second>& _pair)
{
    stream << (boost::format("(%1$s, %2$s)") % _pair.first % _pair.second).str();
    return stream;
}

/**
 * One can use this iterator to output values from container to ostream
 * with std::copy without delimiter after last element
 **/
template <class T, class charT = char, class traits = std::char_traits<charT>>
class InfixOstreamIterator
  : public std::iterator<std::output_iterator_tag, void, void, void, void>
{
public:
    using char_type = charT;
    using traits_type = traits;
    using ostream_type = std::basic_ostream<charT, traits>;

    explicit InfixOstreamIterator(ostream_type& s) : m_os(&s), m_delimiter(0), m_first_elem(true)
    {
    }

    InfixOstreamIterator(ostream_type& s, charT const* d) :
      m_os(&s), m_delimiter(d), m_first_elem(true)
    {
    }

    InfixOstreamIterator<T, charT, traits>& operator=(T const& item)
    {
        // Here's the only real change from ostream_iterator:
        // Normally, the '*os << item;' would come before the 'if'.
        if (!m_first_elem && m_delimiter != 0)
        {
            *m_os << m_delimiter;
        }
        *m_os << item;
        m_first_elem = false;
        return *this;
    }

    InfixOstreamIterator<T, charT, traits>& operator*()
    {
        return *this;
    }

    InfixOstreamIterator<T, charT, traits>& operator++()
    {
        return *this;
    }

    InfixOstreamIterator<T, charT, traits>& operator++(int)
    {
        return *this;
    }

private:
    std::basic_ostream<charT, traits>* m_os;
    charT const* m_delimiter;
    bool m_first_elem;
};

template <typename Container>
inline std::ostream& print(std::ostream& strm, const Container& container, const std::string& name)
{
    strm << name << "(";
    std::copy(
        container.begin(),
        container.end(),
        InfixOstreamIterator<typename Container::value_type>(strm, ", "));
    strm << ")";
    return strm;
}
