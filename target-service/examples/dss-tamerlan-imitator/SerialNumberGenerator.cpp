#include "SerialNumberGenerator.h"

#include <array>

namespace mpk::dss::imitator::tamerlan
{

namespace
{

const std::string alphabet = {
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"};

} // namespace

SerialNumberGenerator::SerialNumberGenerator() :
  m_alphabetDist(0, static_cast<int>(alphabet.size() - 1)),
  m_generator(m_randomDevice())
{
}

std::string mpk::dss::imitator::tamerlan::SerialNumberGenerator::generate(
    int numberLength)
{
    std::string serial;
    serial.reserve(numberLength);

    for (int i = 0; i < numberLength; ++i)
    {
        serial += alphabet.at(m_alphabetDist(m_generator));
    }

    return serial;
}

} // namespace mpk::dss::imitator::tamerlan
