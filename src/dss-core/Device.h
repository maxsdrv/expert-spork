#pragma once

#include "dss-core/DeviceClass.h"
#include "dss-core/DeviceId.h"

#include "log/Log2.h"

#include "gsl/pointers"

#include <memory>

namespace mpk::dss::core
{

template <typename Properties>
class Device
{

public:
    using Id = DeviceId;

    Device(Id id, std::unique_ptr<Properties> impl) :
      m_id(std::move(id)), m_impl{std::move(impl)}
    {
        LOG_DEBUG << "Create device id: "
                  << static_cast<QString>(m_id).toStdString();
    }

    Device(const Device& other) = delete;
    Device& operator=(const Device& other) = delete;

    Device(Device&& other) noexcept :
      m_id{std::move(other.m_id)}, m_impl{std::move(other.m_impl)}
    {
    }

    Device& operator=(Device&& other) noexcept
    {
        m_id = std::move(other.m_id);
        m_impl = std::move(other.m_impl);
        return *this;
    }

    [[nodiscard]] Id id() const
    {
        return m_id;
    }

    [[nodiscard]] Properties& properties() const
    {
        return *m_impl;
    }

private:
    Id m_id;
    std::unique_ptr<Properties> m_impl;
};

} // namespace mpk::dss::core
