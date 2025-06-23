#include "RadarIqCameraControl.h"

#include "log/Log2.h"

namespace mpk::dss::backend::radariq
{
using namespace std::chrono_literals;

RadarIqCameraControl::RadarIqCameraControl(
    QString radarIqHost,
    QObject* parent) :
  QObject{parent},
  m_id(std::move(radarIqHost))
{
}

RadarIqCameraControl::~RadarIqCameraControl() = default;

QString RadarIqCameraControl::id() const
{
    return m_id;
}

} // namespace mpk::dss::backend::radariq
