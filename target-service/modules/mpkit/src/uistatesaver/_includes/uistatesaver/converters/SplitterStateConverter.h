/** @file
 * @brief     Header view state converter specification
 *
 * $Id: $
 */

#pragma once

#include "uistatesaver/StateConverterInjection.h"

class QSplitter;

namespace uistatesaver
{

/**
 * Header view state converter helpers base
 */
class SplitterStateConverterInjection // clazy:exclude=ctor-missing-parent-argument
  : public StateConverterInjection
{
    Q_OBJECT

public:
    SplitterStateConverterInjection(QSplitter* splitter, QString id);

    /**
     * Return state for which widget installed on
     */
    QByteArray state() const override;

    /**
     * Restore widget state
     */
    void restore(const QByteArray& state) override;

    /**
     * Filters events for determine when save/restore state
     */
    bool eventFilter(QObject* object, QEvent* event) override;

private:
    QSplitter* m_splitter;
};

struct SplitterTraits
{
    using ObjectType = QSplitter;
    using InjectionType = SplitterStateConverterInjection;
};

} // namespace uistatesaver
