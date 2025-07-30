#pragma once

#include "boost/signals2.h"

#include <QObject>

namespace rpc
{

class SimpleTimer
{
    ADD_SIGNAL(Triggered, void())

public:
    explicit SimpleTimer(int timeout);

private:
    QObject m_context;
};

} // namespace rpc
