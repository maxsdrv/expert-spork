#include "rpc/transport/IncomingMessage.h"
#include "rpc/transport/Socket.h"
#include "rpc/transport/TransportExceptions.h"
#include "rpc/transport/udp/UdpEndpoint.h"
#include "rpc/transport/udp/UdpMessenger.h"
#include "rpc/transport/udp/UdpTransportCreator.h"

#include "DataReceiver.h"

#include "boost/SignalTracker.h"

#include "json/JsonUtilities.h"

#include "ApplicationFixture.h"

#include <QNetworkDatagram>
#include <QUdpSocket>

#include "gqtest"

namespace rpc
{

using ::testing::Eq;

TEST(UdpTransportTest, createGood)
{
    auto json = json::fromByteArray(
        "{ \"listen\": {\"host\": \"any\",\"port\": 17793},"
        "  \"destination\": {\"host\": \"localhost\", \"port\": 17794} }");
    std::unique_ptr<Transport> transport;
    ASSERT_NO_THROW(transport = createUdpTransport(json));
    EXPECT_NE(nullptr, dynamic_cast<UdpEndpoint*>(transport.get()));
}

TEST(UdpTransportTest, createBad)
{
    auto json = json::fromByteArray(
        "{ \"lissten\": {\"host\": \"any\",\"port\": 17793},"
        "  \"desstination\": {\"host\": \"localhost\", \"port\": 17794} }");
    EXPECT_THROW(createUdpTransport(json), std::exception);
}

TEST(UdpTransportTest, createMissedHost)
{
    auto json = json::fromByteArray(
        "{ \"listen\": {\"port\": 17793},"
        "  \"destination\": {\"host\": \"localhost\", \"port\": 17794} }");
    EXPECT_THROW(createUdpTransport(json), std::exception);
}

TEST(UdpTransportTest, createMissedPort)
{
    auto json = json::fromByteArray(
        "{ \"listen\": {\"host\": \"any\",\"port\": 17793},"
        "  \"destination\": {\"host\": \"localhost\"} }");
    EXPECT_THROW(createUdpTransport(json), std::exception);
}

TEST(UdpTransportTest, messengerSend)
{
    gqtest::ApplicationFixture app;

    QUdpSocket socket;
    UdpMessenger messenger(&socket, {QHostAddress::LocalHost, 10001});

    DataReceiver dataReceiver;
    QByteArray buffer("9876543210");

    QUdpSocket recvSocket;
    recvSocket.bind(QHostAddress::LocalHost, 10001);
    QObject::connect(
        &recvSocket,
        &QUdpSocket::readyRead,
        &dataReceiver,
        [&dataReceiver, &recvSocket](){
            auto datagram = recvSocket.receiveDatagram();
            dataReceiver.receiveData(datagram.data());
        });

    EXPECT_CALL(dataReceiver, received(Eq(buffer))).Times(testing::Exactly(1));
    messenger.send(buffer);

    app.exec(&dataReceiver, &DataReceiver::dataReceived, 100);
}

TEST(UdpTransportTest, messengerReceive)
{
    gqtest::ApplicationFixture app;

    QUdpSocket socket;
    socket.bind(QHostAddress::LocalHost, 10002);
    UdpMessenger messenger(&socket, {QHostAddress::LocalHost, 10003});

    DataReceiver dataReceiver;
    QByteArray buffer("1234567890");
    QObject::connect(
        &messenger, &Messenger::received,
        &dataReceiver, &DataReceiver::receiveData);

    EXPECT_CALL(dataReceiver, received(Eq(buffer))).Times(testing::Exactly(1));
    QUdpSocket sendSocket;
    socket.writeDatagram(buffer, QHostAddress::LocalHost, 10002);

    app.exec(&dataReceiver, &DataReceiver::dataReceived, 100);
}

TEST(UdpTransportTest, endpointSend)
{
    gqtest::ApplicationFixture app;

    UdpEndpoint udpEndpoint(
        {QHostAddress::LocalHost, 10004}, {QHostAddress::LocalHost, 10005});

    DataReceiver dataReceiver;
    QByteArray buffer("9876543210");

    QUdpSocket recvSocket;
    recvSocket.bind(QHostAddress::LocalHost, 10004);
    QObject::connect(
        &recvSocket,
        &QUdpSocket::readyRead,
        &dataReceiver,
        [&dataReceiver, &recvSocket](){
            auto datagram = recvSocket.receiveDatagram();
            dataReceiver.receiveData(datagram.data());
        });

    EXPECT_CALL(dataReceiver, received(Eq(buffer))).Times(testing::Exactly(1));
    udpEndpoint.send(buffer);

    app.exec(&dataReceiver, &DataReceiver::dataReceived, 100);
}

TEST(UdpTransportTest, endpointReceive)
{
    gqtest::ApplicationFixture app;

    UdpEndpoint udpEndpoint(
        {QHostAddress::LocalHost, 10006}, {QHostAddress::LocalHost, 10007});

    DataReceiver dataReceiver;
    QByteArray buffer("1234567890");
    boostsignals::SignalTracker guard;
    udpEndpoint.onReceived([&dataReceiver](const auto& message){
        dataReceiver.receiveData(message->buffer());
    }, guard());

    EXPECT_CALL(dataReceiver, received(Eq(buffer))).Times(testing::Exactly(1));
    QUdpSocket sendSocket;
    sendSocket.writeDatagram(buffer, QHostAddress::LocalHost, 10007);

    app.exec(&dataReceiver, &DataReceiver::dataReceived, 100);
}

} // namespace rpc

