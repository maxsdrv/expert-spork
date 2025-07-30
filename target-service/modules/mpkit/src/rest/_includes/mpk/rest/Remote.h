#pragma once

#include "mpk/stable_link/Informer.h"

#include "mpk/rest/Synchronizer.h"

namespace mpk::rest
{

class Remote : public stable_link::Informer, public Synchronizer
{
};

} // namespace mpk::rest
