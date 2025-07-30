#pragma once

#include <complex>
#include <cstdint>
#include <vector>

#include <boost/align/aligned_allocator.hpp>

namespace mpk::signal_scanner::detector
{

using t32fc = std::complex<float>;
using t16sc = std::complex<int16_t>;

static const size_t maxAlignment = 32;
template <typename T>
using alignedVector =
    std::vector<T, boost::alignment::aligned_allocator<T, maxAlignment>>;

using vector32fc = alignedVector<t32fc>;
using vector32f = alignedVector<float>;
using vector32s = alignedVector<int32_t>;

using FrequencyKHz = int32_t;
using TimeMks = int32_t;

} // namespace mpk::signal_scanner::detector