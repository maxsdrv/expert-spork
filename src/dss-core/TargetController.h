#pragma once

#include <QObject>

#include "dss-core/TargetData.h"

#include "boost/iterators/ForwardIterator.h"

#include "gsl/pointers"

#include <vector>

namespace mpk::dss::core
{

class TrackController;

class TargetController : public QObject
{
    Q_OBJECT

public:
    explicit TargetController(
        gsl::not_null<const TrackController*> trackController,
        QObject* parent = nullptr);

    using Iterator = mpkit::ForwardIterator<TargetData const>;

    [[nodiscard]] Iterator begin() const;
    [[nodiscard]] Iterator end() const;
    [[nodiscard]] Iterator find(const TargetId& targetId) const;

signals:
    void detected(mpk::dss::core::TargetId);
    void updated(mpk::dss::core::TargetId);
    void lost(mpk::dss::core::TargetId);

private:
    gsl::not_null<const TrackController*> m_trackController;
    std::vector<TargetData> m_targets;
};

} // namespace mpk::dss::core
