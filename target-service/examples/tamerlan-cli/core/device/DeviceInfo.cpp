#include "DeviceInfo.h"

namespace mpk::drone::core
{

DeviceInfoHolder::DeviceInfoHolder(DeviceInfo info, QObject* parent) :
  QObject(parent), m_info(std::move(info))
{
}

DeviceInfo DeviceInfoHolder::info() const
{
    return m_info;
}

} // namespace mpk::drone::core
