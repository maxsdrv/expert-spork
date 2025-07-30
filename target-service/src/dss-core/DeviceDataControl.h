#pragma once

#include <QString>

namespace mpk::dss::core
{

class DeviceDataControl
{
public:
    virtual ~DeviceDataControl() = default;

    using Serial = QString;
    using Model = QString;
    using Version = QString;
    using Fingerprint = QString;

    [[nodiscard]] virtual Serial serial() const = 0;
    [[nodiscard]] virtual Model model() const = 0;
    [[nodiscard]] virtual Version swVersion() const = 0;
    [[nodiscard]] virtual Fingerprint fingerprint() const = 0;
};

} // namespace mpk::dss::core
