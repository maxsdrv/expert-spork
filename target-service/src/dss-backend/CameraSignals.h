#pragma once

#include "dss-core/DeviceId.h"

namespace mpk::dss::backend
{

class CameraSignals : public QObject
{
    Q_OBJECT

public:
    explicit CameraSignals(QObject* parent = nullptr) : QObject(parent)
    {
    }
};

} // namespace mpk::dss::backend
