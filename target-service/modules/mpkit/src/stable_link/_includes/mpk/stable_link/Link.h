#pragma once

#include "mpk/stable_link/Informer.h"

namespace mpk::stable_link
{

class Link : public Informer
{
public:
    virtual void switchToOnline() = 0;
    virtual void switchToOffline() = 0;
};

} // namespace mpk::stable_link
