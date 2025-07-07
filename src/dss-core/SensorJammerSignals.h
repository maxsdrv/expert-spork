#pragma once

#include "JammerMode.h"

#include <QMetaType>
#include <QObject>

#include <chrono>

namespace mpk::dss::core
{

using Seconds = std::chrono::seconds;

class SensorJammerSignals : public QObject
{
    Q_OBJECT

public:
    explicit SensorJammerSignals(QObject* parent = nullptr) : QObject(parent)
    {
    }

    void emitJammerDataChanged()
    {
        emit jammerDataChanged();
    }

signals:
    void jammerDataChanged();
};

} // namespace mpk::dss::core

Q_DECLARE_METATYPE(mpk::dss::core::Seconds)
