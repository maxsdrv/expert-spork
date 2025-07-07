#include "Geolocation.h"

#include <istream>

namespace
{

template <typename C, typename T>
std::basic_istream<C, T>& skipUntilDigit(std::basic_istream<C, T>& inputStream)
{
    const auto& facetLocale =
        std::use_facet<std::ctype<C>>(inputStream.getloc());
    auto* currentStreamBuffer = inputStream.rdbuf();

    auto currentChar = currentStreamBuffer->sgetc();
    while (currentChar != T::eof() && currentChar != '-'
           && !facetLocale.is(std::ctype_base::digit, currentChar))
    {
        currentChar = currentStreamBuffer->snextc();
    }
    if (currentChar == T::eof())
    {
        inputStream.setstate(std::ios_base::eofbit);
    }
    return inputStream;
}

} // namespace

std::ostream& operator<<(std::ostream& os, const QGeoCoordinate& value)
{
    return os << value.latitude() << ',' << value.longitude() << ','
              << value.altitude();
}

std::istream& operator>>(std::istream& is, QGeoCoordinate& into)
{
    auto paramsList = {
        &QGeoCoordinate::setLatitude,
        &QGeoCoordinate::setLongitude,
        &QGeoCoordinate::setAltitude};

    for (const auto& param: paramsList)
    {
        auto value = 0.;
        if (!is.eof())
        {
            is >> skipUntilDigit >> value;
        }
        (into.*param)(value);
    }

    return is;
}
