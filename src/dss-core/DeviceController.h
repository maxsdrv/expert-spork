#pragma once

#include "dss-core/Device.h"
#include "dss-core/DeviceProvider.h"

#include "gsl/pointers"

#include <vector>

namespace mpk::dss::core
{

// TODO: implement
template <typename Properties>
class DeviceController
{

public:
    explicit DeviceController(
        std::vector<gsl::not_null<DeviceProvider<Properties>*>> providers);

    using Iterator = mpkit::ForwardIterator<Device<Properties>>;

    [[nodiscard]] Iterator begin() const;
    [[nodiscard]] Iterator end() const;
    [[nodiscard]] Iterator find(const DeviceId& deviceId) const;

private:
    std::vector<gsl::not_null<DeviceProvider<Properties>*>> m_providers;
};

} // namespace mpk::dss::core
