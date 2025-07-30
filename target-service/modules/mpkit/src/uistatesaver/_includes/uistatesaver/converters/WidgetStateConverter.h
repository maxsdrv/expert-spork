/** @file
 * @brief     Widget state converter specifications
 *
 * $Id: $
 */

#pragma once

#include "uistatesaver/StateConverterInjection.h"

namespace uistatesaver
{

/**
 * Helper for widget size converter. Intercept information from it's widget
 */
class WidgetStateConverterInjection // clazy:exclude=ctor-missing-parent-argument
  : public StateConverterInjection
{
    Q_OBJECT

public:
    WidgetStateConverterInjection(QWidget* widget, QString id);

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
    QWidget* m_widget;
};

struct WidgetTraits
{
    using ObjectType = QWidget;
    using InjectionType = WidgetStateConverterInjection;
};

} // namespace uistatesaver
