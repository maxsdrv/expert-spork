#pragma once

#include "JammerBandOption.h"
#include "JammerMode.h"

#include <QObject>

namespace mpk::dss::core
{

class JammerBandSignals : public QObject
{
    Q_OBJECT

public:
    explicit JammerBandSignals(QObject* parent = nullptr) : QObject(parent)
    {
    }

signals:
    void bandsChanged();
};

} // namespace mpk::dss::core
