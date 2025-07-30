#include "TransportMock.h"

#include "rpc/RpcServer.h"

#include "rpc/proto/JsonProtocol.h"

#include "qt/Strings.h"

#include "gqtest"

namespace rpc
{

QString addAndFormat(int16_t a, int16_t b, QString format)
{
    return format.arg(a + b);
}

class RpcServerTest : public ::testing::Test
{
public:
    RpcServerTest() :
      m_transport(new TransportMock()),
      m_protocol(DataFormat::Json),
      m_server(std::unique_ptr<TransportMock>(m_transport), &m_handlers, m_protocol)
    {
    }

protected:
    TransportMock* m_transport;
    Protocol m_protocol;
    RpcHandlers<Protocol> m_handlers;
    RpcServer<Protocol> m_server;
};

TEST_F(RpcServerTest, testDuplicateHandler)
{
    m_handlers.add(MethodName("addAndFormat"), addAndFormat);

    EXPECT_THROW(
        m_handlers.add(MethodName("addAndFormat"), addAndFormat),
        exception::DuplicateEntity);
}

TEST_F(RpcServerTest, testMethodWithReturnValue)
{
    RequestId id{120};
    MethodName method{"addAndFormat"};
    m_handlers.add(method, addAndFormat);

    auto buffer =
        m_protocol.serializer()
            .packValue(rpc::tag::id, id)
            .packValue(rpc::tag::method, method)
            .packArray(
                rpc::tag::args, int16_t{10}, int16_t{15}, QString("Answer: %1"))
            .buffer();

    m_transport->receive(buffer);

    RequestId id_;
    ReturnCode code_;
    QString result_;
    m_protocol.deserializer(m_transport->buffer())
        .unpackValue(rpc::tag::id, id_)
        .unpackValue(rpc::tag::code, code_)
        .unpackValue(rpc::tag::result, result_);

    EXPECT_EQ(id, id_);
    EXPECT_EQ(ReturnCode::SUCCESS, code_);
    EXPECT_EQ(QString("Answer: 25"), result_);
}

TEST_F(RpcServerTest, testMethodReturnsVoid)
{
    RequestId id{120};
    MethodName method{"voidMethod"};
    m_handlers.add(method, []() {});

    auto buffer = m_protocol.serializer()
                      .packValue(rpc::tag::id, id)
                      .packValue(rpc::tag::method, method)
                      .packArray(rpc::tag::args)
                      .buffer();

    m_transport->receive(buffer);

    RequestId id_;
    ReturnCode code_;

    m_protocol.deserializer(m_transport->buffer())
        .unpackValue(rpc::tag::id, id_)
        .unpackValue(rpc::tag::code, code_);

    EXPECT_EQ(id, id_);
    EXPECT_EQ(ReturnCode::SUCCESS, code_);
}

TEST_F(RpcServerTest, testCallWithInvalidParams)
{
    RequestId id{120};
    MethodName method{"addAndFormat"};
    m_handlers.add(method, addAndFormat);

    auto buffer = m_protocol.serializer()
                      .packValue(rpc::tag::id, id)
                      .packValue(rpc::tag::method, method)
                      .packArray(rpc::tag::args, int16_t{10}, int16_t{15})
                      .buffer();

    m_transport->receive(buffer);

    RequestId id_;
    ReturnCode code_;
    std::string error_;
    m_protocol.deserializer(m_transport->buffer())
        .unpackValue(rpc::tag::id, id_)
        .unpackValue(rpc::tag::code, code_)
        .unpackValue(rpc::tag::error, error_);

    EXPECT_EQ(id, id_);
    EXPECT_EQ(ReturnCode::INVALID_ARGS, code_);
    EXPECT_EQ(method, error_);
}

TEST_F(RpcServerTest, testCallInvalidMethod)
{
    RequestId id{120};
    MethodName method{"addAndFormat"};

    auto buffer = m_protocol.serializer()
                      .packValue(rpc::tag::id, id)
                      .packValue(rpc::tag::method, method)
                      .buffer();

    m_transport->receive(buffer);

    RequestId id_;
    ReturnCode code_;
    std::string error_;
    m_protocol.deserializer(m_transport->buffer())
        .unpackValue(rpc::tag::id, id_)
        .unpackValue(rpc::tag::code, code_)
        .unpackValue(rpc::tag::error, error_);

    EXPECT_EQ(id, id_);
    EXPECT_EQ(ReturnCode::INVALID_METHOD, code_);
    EXPECT_EQ(method, error_);
}

TEST_F(RpcServerTest, testMethodWhichThrows)
{
    RequestId id{120};
    MethodName method{"methodThrows"};

    m_handlers.add(method, []() { throw std::runtime_error("error"); });

    auto buffer = m_protocol.serializer()
                      .packValue(rpc::tag::id, id)
                      .packValue(rpc::tag::method, method)
                      .packArray(rpc::tag::args)
                      .buffer();

    m_transport->receive(buffer);

    RequestId id_;
    ReturnCode code_;
    std::string error_;
    m_protocol.deserializer(m_transport->buffer())
        .unpackValue(rpc::tag::id, id_)
        .unpackValue(rpc::tag::code, code_)
        .unpackValue(rpc::tag::error, error_);

    EXPECT_EQ(id, id_);
    EXPECT_EQ(ReturnCode::INTERNAL_ERROR, code_);
    EXPECT_EQ("error", error_);
}

} // namespace rpc
