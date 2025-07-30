#pragma once

#include "core/Geo.h"

#include <QObject>

namespace mpk::drone::core
{

struct GeoData
{
    Position position;
    Angle azimuth;
};

inline bool operator==(const GeoData& left, const GeoData& right);
inline bool operator!=(const GeoData& left, const GeoData& right);

class GeoDataHolder : public QObject
{
    Q_OBJECT

public:
    explicit GeoDataHolder(const GeoData& data, QObject* parent = nullptr);

    void setData(const GeoData& data);
    [[nodiscard]] GeoData data() const;

    void setPosition(const Position& position);
    [[nodiscard]] Position position() const;

    void setAzimuth(Angle azimuth);
    [[nodiscard]] Angle azimuth() const;

signals:
    void changed(GeoData);

private:
    GeoData m_data;
};

} // namespace mpk::drone::core

Q_DECLARE_METATYPE(mpk::drone::core::GeoData);
