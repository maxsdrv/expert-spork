#pragma once

#include "boost/SignalTracker.h"
#include "boost/signals2.h"

#include "rpc/Types.h"

namespace rpc
{

class RequestTrackerBase
{
    template <typename ProtocolType, typename Timer>
    friend class RpcClient;

    ADD_SIGNAL_T(Error, void(ReturnCode, std::string))
    ADD_SIGNAL_T(Complete, void())
    ADD_SIGNAL_T(Destroy, void())

public:
    explicit RequestTrackerBase(RequestId id) : m_id(id)
    {
    }

    [[nodiscard]] RequestId id() const
    {
        return m_id;
    }

    [[nodiscard]] ReturnCode code() const
    {
        return m_code;
    }

    [[nodiscard]] bool succeeded() const
    {
        return m_code == ReturnCode::SUCCESS;
    }

    [[nodiscard]] std::string errorString() const
    {
        return m_errorString;
    }

    virtual ~RequestTrackerBase()
    {
        m_Destroy();
    }

protected:
    void setError(ReturnCode code, const std::string& errorString)
    {
        m_errorString = errorString;
        m_code = code;
        m_Error(code, errorString);
        m_Complete();
    }

protected:
    // NOLINTNEXTLINE
    ReturnCode m_code = ReturnCode::UNDEFINED;

private:
    RequestId m_id;
    std::string m_errorString;
};

template <class ReturnType>
class RequestTracker : public RequestTrackerBase
{
    ADD_SIGNAL_T(Success, void(ReturnType))

    template <typename ProtocolType, typename Timer>
    friend class RpcClient;

public:
    explicit RequestTracker(RequestId id) : RequestTrackerBase(id)
    {
    }

    [[nodiscard]] ReturnType result() const
    {
        return m_result;
    }

protected:
    void setSuccess(const ReturnType& value)
    {
        m_result = value;
        m_code = ReturnCode::SUCCESS;
        m_Success(value);
        m_Complete();
    }

private:
    ReturnType m_result;
};

template <>
class RequestTracker<void> : public RequestTrackerBase
{
    ADD_SIGNAL_T(Success, void())

    template <typename ProtocolType, typename Timer>
    friend class RpcClient;

public:
    explicit RequestTracker(RequestId id) : RequestTrackerBase(id)
    {
    }

    void result() const
    {
        // Do nothing function
    }

protected:
    void setSuccess()
    {
        m_code = ReturnCode::SUCCESS;
        m_Success();
        m_Complete();
    }
};

} // namespace rpc
