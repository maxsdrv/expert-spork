#include "DeviceRegistrator.h"

#include "log/Log2.h"

#include "qt/DeferredCall.h"

namespace mpk::dss::core
{

using namespace std::chrono_literals;
constexpr auto minRetryBackoff = 1s;
constexpr auto maxRetryBackoff = std::chrono::milliseconds(5s);

DeviceRegistrator::DeviceRegistrator(
    std::shared_ptr<DeviceIdMappingStorage> deviceIdMapping,
    gsl::not_null<core::DeviceDataControl*> dataControl,
    DeviceClass::Value deviceClass,
    QObject* parent) :
  QObject(parent),
  m_retryBackoff{minRetryBackoff},
  m_deviceIdMapping{std::move(deviceIdMapping)},
  m_dataControl{dataControl},
  m_deviceClass(deviceClass),
  m_guard{std::make_shared<bool>()}
{
    LOG_DEBUG << "Device id initial registration";
    tryRegister();
}

void DeviceRegistrator::tryRegister()
{
    // auto timeout = m_retryBackoff;
    auto* deferredCall = new qt::DeferredCall{
        [this]() { tryRegisterMain(); },
        m_guard,
        static_cast<int>(m_retryBackoff.count())};
    deferredCall->runFree();

    m_retryBackoff = std::min(m_retryBackoff * 2, maxRetryBackoff);
}

void DeviceRegistrator::tryRegisterMain()
{
    auto fingerprint = m_dataControl->fingerprint();
    if (!fingerprint.isEmpty())
    {
        DeviceId id;
        auto mappingKey = DeviceIdMappingKey{fingerprint, m_deviceClass};

        auto value =
            m_deviceIdMapping->find(DeviceIdMappingKey::toString(mappingKey));
        if (value)
        {
            LOG_DEBUG << "Device id mapping found record for fingerprint ["
                      << fingerprint << "] and class "
                      << DeviceClass::toString(m_deviceClass) << " with id "
                      << value->id;
            id = value->id;
        }
        else
        {
            LOG_DEBUG << "Device id mapping not found for fingerprint ["
                      << fingerprint << "] and class "
                      << DeviceClass::toString(m_deviceClass);
            LOG_DEBUG << "Device id mapping fingerprint forced insert";
            id = DeviceId::generate();
            m_deviceIdMapping->modifier()->append(DeviceIdMapping{
                .key = DeviceIdMappingKey::toString(
                    DeviceIdMappingKey{fingerprint, m_deviceClass}),
                .id = id});
            LOG_DEBUG << "Device id mapping: added record with id " << id
                      << "for fingerprint [" << fingerprint << "] and class "
                      << DeviceClass::toString(m_deviceClass);
        }

        LOG_DEBUG << "Device id registered: " << id;
        emit registered(id);
        return;
    }
    LOG_WARNING
        << "Device id registration is unsuccessful. Retry after "
        << std::chrono::duration_cast<std::chrono::seconds>(m_retryBackoff)
               .count()
        << " s";

    tryRegister();
}

} // namespace mpk::dss::core
