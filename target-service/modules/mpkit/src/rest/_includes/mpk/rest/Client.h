#pragma once

#include "MessageSubscriber.h"
#include "OperationCaller.h"

#include "mpk/stable_link/Informer.h"

namespace mpk::rest
{

class Client : public OperationCaller, public MessageSubscriber, public stable_link::Informer
{
};

} // namespace mpk::rest
