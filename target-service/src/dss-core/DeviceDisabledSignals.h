#pragma once

#include <QObject>

namespace mpk::dss::core
{

class DeviceDisabledSignals : public QObject
{
    Q_OBJECT

public:
    explicit DeviceDisabledSignals(QObject* parent = nullptr) : QObject(parent)
    {
    }

signals:
    void disabledChanged(bool);
};

} // namespace mpk::dss::core
