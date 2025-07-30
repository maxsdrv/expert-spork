#pragma once

#include <QLineF>
#include <QPointF>
#include <QRectF>
#include <QSizeF>

#include <concepts>

namespace mpk::concepts
{

template <typename T>
concept QtLine = std::same_as<T, QLine> || std::same_as<T, QLineF>;

template <typename T>
concept QtPoint = std::same_as<T, QPoint> || std::same_as<T, QPointF>;

template <typename T>
concept QtRect = std::same_as<T, QRect> || std::same_as<T, QRectF>;

template <typename T>
concept QtSize = std::same_as<T, QSize> || std::same_as<T, QSizeF>;

} // namespace mpk::concepts
