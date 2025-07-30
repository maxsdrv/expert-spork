#pragma once

#include "mpk/stable_link/State.h"

#include <QObject>

Q_DECLARE_METATYPE(mpk::stable_link::State)

namespace mpk::stable_link
{

class Signals : public QObject
{
    Q_OBJECT

public:
    explicit Signals(QObject* parent = nullptr) : QObject{parent}
    {
        qRegisterMetaType<State>();
    }

signals:
    void linkStateChanged(mpk::stable_link::State state);
};

} // namespace mpk::stable_link
