#include "utils/RandomString.h"

#include <algorithm>
#include <random>
#include <string_view>

namespace utils
{

std::string randomLatinString(std::size_t size)
{
    static std::string_view charset =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    std::random_device generator;
    std::mt19937 engine(generator());

    std::string result;
    result.reserve(size);
    std::generate_n(
        std::back_inserter(result),
        size,
        [&engine]() { return charset[engine() % (charset.size() - 1)]; });
    return result;
}

} // namespace utils
