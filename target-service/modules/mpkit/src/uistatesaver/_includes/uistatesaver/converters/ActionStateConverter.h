/** @file
 * @brief     Action state converter specification
 *
 * $Id: $
 */

#pragma once

#include "uistatesaver/StateConverterInjection.h"

class QAction;

namespace uistatesaver
{

class ActionStateConverterInjection // clazy:exclude=ctor-missing-parent-argument
  : public StateConverterInjection
{
    Q_OBJECT

public:
    ActionStateConverterInjection(QAction* action, QString id);

    /**
     * Return state for which widget installed on
     */
    QByteArray state() const override;

    /**
     * Restore widget state
     */
    void restore(const QByteArray& state) override;

private:
    QAction* m_action;
};

struct ActionTraits
{
    using ObjectType = QAction;
    using InjectionType = ActionStateConverterInjection;
};

} // namespace uistatesaver
