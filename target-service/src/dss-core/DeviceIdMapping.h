#pragma once

#include "dss-core/DeviceClass.h"
#include "dss-core/DeviceDataControl.h"
#include "dss-core/DeviceId.h"

namespace mpk::dss::core
{

struct DeviceIdMappingKey
{
    DeviceDataControl::Fingerprint fingerprint;
    DeviceClass::Value type;

    static QString toString(const DeviceIdMappingKey& key);
    static DeviceIdMappingKey fromString(const QString& str);
};

struct DeviceIdMapping
{
    QString key;
    DeviceId id;
};

bool operator==(const DeviceIdMapping& left, const DeviceIdMapping& right);
bool operator!=(const DeviceIdMapping& left, const DeviceIdMapping& right);

} // namespace mpk::dss::core
