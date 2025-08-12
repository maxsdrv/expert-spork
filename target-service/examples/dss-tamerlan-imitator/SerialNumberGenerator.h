#pragma once

#include <random>
#include <string>

namespace mpk::dss::imitator::tamerlan
{

class SerialNumberGenerator
{
public:
    SerialNumberGenerator();

    std::string generate(int numberLength);

private:
    std::uniform_int_distribution<> m_alphabetDist;
    std::random_device m_randomDevice;
    mutable std::mt19937 m_generator;
};

} // namespace mpk::dss::imitator::tamerlan
