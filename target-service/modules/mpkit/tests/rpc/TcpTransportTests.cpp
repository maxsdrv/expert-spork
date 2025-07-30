#include "rpc/transport/IncomingMessage.h"
#include "rpc/transport/Socket.h"
#include "rpc/transport/TransportExceptions.h"
#include "rpc/transport/tcp/TcpClient.h"
#include "rpc/transport/tcp/TcpMultiServer.h"
#include "rpc/transport/tcp/TcpSingleServer.h"
#include "rpc/transport/tcp/TcpTransportCreator.h"

#include "DataReceiver.h"

#include "boost/SignalTracker.h"

#include "json/JsonUtilities.h"

#include "ApplicationFixture.h"

#include <QTcpServer>
#include <QTcpSocket>

#include "gqtest"

namespace rpc
{

using ::testing::Eq;

TEST(TcpTransportTest, createClientGood)
{
    auto json = json::fromByteArray(
        "{ \"mode\": \"client\", \"host\": \"localhost\", \"port\": 17797}");
    std::unique_ptr<Transport> transport;
    ASSERT_NO_THROW(transport = createTcpTransport(json));
    EXPECT_NE(nullptr, dynamic_cast<TcpClient*>(transport.get()));
}

TEST(TcpTransportTest, createSingleServerGood)
{
    auto json = json::fromByteArray(
        "{ \"mode\": \"single_server\", \"host\": \"localhost\", \"port\": 17798}");
    std::unique_ptr<Transport> transport;
    ASSERT_NO_THROW(transport = createTcpTransport(json));
    EXPECT_NE(nullptr, dynamic_cast<TcpSingleServer*>(transport.get()));
}

TEST(TcpTransportTest, createMultiServerGood)
{
    auto json = json::fromByteArray(
        "{ \"mode\": \"multi_server\", \"host\": \"localhost\", \"port\": 17799}");
    std::unique_ptr<Transport> transport;
    ASSERT_NO_THROW(transport = createTcpTransport(json));
    EXPECT_NE(nullptr, dynamic_cast<TcpMultiServer*>(transport.get()));
}

TEST(TcpTransportTest, createBad)
{
    auto json = json::fromByteArray(
        "{ \"moose\": \"client\", \"hvost\": \"localhost\", \"portage\": 17798}");
    EXPECT_THROW(createTcpTransport(json), std::exception);
}

TEST(TcpTransportTest, createMissedMode)
{
    auto json = json::fromByteArray(
        "{ \"host\": \"localhost\", \"port\": 17798}");
    EXPECT_THROW(createTcpTransport(json), std::exception);
}

TEST(TcpTransportTest, createMissedHost)
{
    auto json = json::fromByteArray(
        "{ \"mode\": \"client\", \"port\": 17798}");
    EXPECT_THROW(createTcpTransport(json), std::exception);
}

TEST(TcpTransportTest, createMissedPort)
{
    auto json = json::fromByteArray(
        "{ \"mode\": \"single_Server\", \"host\": \"localhost\"}");
    EXPECT_THROW(createTcpTransport(json), std::exception);
}

TEST(TcpTransportTest, createInvalidMode)
{
    auto json = json::fromByteArray(
        "{ \"mode\": \"goose\", \"host\": \"localhost\", \"port\": 17798}");
    EXPECT_THROW(createTcpTransport(json), std::exception);
}

TEST(TcpTransportTest, singleServerSendNotConnected)
{
    TcpSingleServer tcpServer({QHostAddress::LocalHost, 11002});
    EXPECT_THROW(tcpServer.send("12345"), exception::TransportNotConnected);
}

TEST(TcpTransportTest, multiServerSend)
{
    TcpMultiServer tcpServer({QHostAddress::LocalHost, 11003});
    EXPECT_THROW(tcpServer.send("12345"), exception::TransportMethodNotSupported);
}

template <typename Server>
class TcpServerTest : public testing::Test
{
protected:
    std::unique_ptr<Server> m_server;
};

TYPED_TEST_SUITE_P(TcpServerTest);


TYPED_TEST_P(TcpServerTest, serverReceive)
{
    gqtest::ApplicationFixture app;

    auto address = SocketAddress{QHostAddress::LocalHost, 11004};
    TcpClient tcpClient(address);

    DataReceiver dataReceiver;
    this->m_server = std::make_unique<TypeParam>(address);
    boostsignals::SignalTracker guard;
    this->m_server->onReceived([&dataReceiver](const auto& message){
        dataReceiver.receiveData(message->buffer());
    }, guard());

    QByteArray buffer("1234567890");
    EXPECT_CALL(dataReceiver, received(Eq(buffer))).Times(testing::Exactly(1));
    tcpClient.send(buffer);

    app.exec(&dataReceiver, &DataReceiver::dataReceived, 1000);
}

TYPED_TEST_P(TcpServerTest, serverReply)
{
    gqtest::ApplicationFixture app;

    DataReceiver dataReceiver;
    boostsignals::SignalTracker guard;
    auto address = SocketAddress{QHostAddress::LocalHost, 11005};
    TcpClient tcpClient(address);
    tcpClient.onReceived([&dataReceiver](const auto& message){
        dataReceiver.receiveData(message->buffer());
    }, guard());

    this->m_server = std::make_unique<TypeParam>(address);
    this->m_server->onReceived([](const auto& message){
        message->reply(message->buffer().repeated(2));
    }, guard());

    QByteArray buffer("1234567890");
    EXPECT_CALL(dataReceiver, received(Eq(buffer.repeated(2)))).Times(testing::Exactly(1));
    tcpClient.send(buffer);

    app.exec(&dataReceiver, &DataReceiver::dataReceived, 1000);
}

REGISTER_TYPED_TEST_SUITE_P(
    TcpServerTest,
    serverReceive,
    serverReply);

using SyncTypes = ::testing::Types<TcpSingleServer, TcpMultiServer>;
INSTANTIATE_TYPED_TEST_SUITE_P(My, TcpServerTest, SyncTypes);

} // namespace rpc

