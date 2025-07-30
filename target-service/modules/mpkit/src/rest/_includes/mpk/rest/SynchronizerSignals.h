#pragma once

#include <QObject>

namespace mpk::rest
{

class SynchronizerSignals : public QObject
{
    Q_OBJECT

public:
    explicit SynchronizerSignals(QObject* parent = nullptr) : QObject{parent}
    {
    }

signals:
    void syncedChanged(bool synced);
};

} // namespace mpk::rest
