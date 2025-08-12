#pragma once

#include <QObject>

namespace mpk::drone::core
{

struct NetworkData
{
    QString host;
    QString netmask;
    QString gateway;
    QString dns;
    bool useDhcp = false;
    bool useDnsFromDhcp = false;
};

inline bool operator==(const NetworkData& left, const NetworkData& right);
inline bool operator!=(const NetworkData& left, const NetworkData& right);

class NetworkDataHolder : public QObject
{
    Q_OBJECT

public:
    explicit NetworkDataHolder(
        NetworkData data, int port, QObject* parent = nullptr);

    void setData(const NetworkData& data);
    [[nodiscard]] NetworkData data() const;

    void setPort(int port);
    [[nodiscard]] int port() const;

signals:
    void dataChanged(NetworkData);
    void portChanged(int);

private:
    NetworkData m_data;
    int m_port = 0;
};

} // namespace mpk::drone::core

Q_DECLARE_METATYPE(mpk::drone::core::NetworkData);
