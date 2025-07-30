#pragma once

#include "dss-core/JammerDevice.h"

#include <QObject>

namespace mpk::dss::core
{

class JammerNotifier : public QObject
{
    Q_OBJECT

public:
    explicit JammerNotifier(
        std::shared_ptr<JammerProviderPtrs> providers,
        QObject* parent = nullptr);

private:
    void connectJammer(const JammerDevice& device);
    [[nodiscard]] JammerDevice* findDevice(const DeviceId& deviceId) const;

private slots:
    void notifyJammer(const DeviceId& id);

signals:
    void notify(QString, QJsonObject);

private:
    std::shared_ptr<JammerProviderPtrs> m_providers;
};

} // namespace mpk::dss::core
