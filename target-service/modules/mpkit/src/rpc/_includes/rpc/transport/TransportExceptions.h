#pragma once

#include "exception/General.h"

namespace rpc
{

namespace exception
{

using TransportError = boost::error_info<struct transportError_, std::string>;

class TransportException : public ::exception::General
{
public:
    explicit TransportException(const std::string& error)
    {
        (*this) << TransportError(error);
    }
};

class TransportCreationFailed : public TransportException
{
public:
    using TransportException::TransportException;
};

class TransportNotConnected : public TransportException
{
public:
    using TransportException::TransportException;
};

class TransportMethodNotSupported : public TransportException
{
public:
    using TransportException::TransportException;
};

class TransportSslException : public TransportException
{
public:
    using TransportException::TransportException;
};

} // namespace exception

} // namespace rpc
