/** @file
 * @brief     Header view state converter specification
 *
 * $Id: $
 */

#pragma once

#include "uistatesaver/StateConverterInjection.h"

class QHeaderView;

namespace uistatesaver
{

/**
 * Header view state converter helper
 */
class HeaderViewStateConverterInjection // clazy:exclude=ctor-missing-parent-argument
  : public StateConverterInjection
{
    Q_OBJECT

public:
    HeaderViewStateConverterInjection(QHeaderView* header, QString id);

    /**
     * Filters events for determine when save/restore state
     */
    bool eventFilter(QObject* object, QEvent* event) override;

    /**
     * Return state for which widget installed on
     */
    QByteArray state() const override;

    /**
     * Restore widget state
     */
    void restore(const QByteArray& state) override;

private:
    QHeaderView* m_header;
};

struct HeaderTraits
{
    using ObjectType = QHeaderView;
    using InjectionType = HeaderViewStateConverterInjection;
};

} // namespace uistatesaver
