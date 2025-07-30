#include "rpc/transport/IncomingMessage.h"
#include "rpc/transport/TransportExceptions.h"
#include "rpc/transport/ssl/SslClient.h"
#include "rpc/transport/ssl/SslData.h"
#include "rpc/transport/ssl/SslMultiServer.h"
#include "rpc/transport/ssl/SslSingleServer.h"
#include "rpc/transport/ssl/SslTransportCreator.h"

#include "DataReceiver.h"

#include "boost/SignalTracker.h"

#include "ApplicationFixture.h"

#include "appkit/Paths.h"

#include "gqtest"

#include <QFile>
#include <QThread>

namespace rpc::tests
{

using ::testing::Eq;

QByteArray readFileContent(const QString& filename)
{
    QFile file(filename);
    if (!filename.isEmpty() && !file.open(QIODevice::ReadOnly))
    {
        return QByteArray();
    }
    return file.readAll();
}

SslData sslData()
{
    return {
        QSslKey(readFileContent(QString::fromStdString(appkit::configFile("key.pem"))), QSsl::Rsa),
        QSslCertificate(readFileContent(QString::fromStdString(appkit::configFile("cert.pem")))),
        QSslCertificate(
            readFileContent(QString::fromStdString(appkit::configFile("ca_cert.pem"))))};
};

class ClientThread : public QThread
{
public:
    ClientThread(
        SocketAddress address,
        SslData sslData,
        QByteArray buffer,
        DataReceiver* dataReceiver = nullptr) :
      m_address(std::move(address)),
      m_sslData(sslData),
      m_buffer(buffer),
      m_dataReceiver(dataReceiver)
    {
    }

protected:
    void run() override
    {
        SslClient sslClient(m_address, m_sslData);
        boostsignals::SignalTracker guard;
        if (m_dataReceiver)
        {
            sslClient.onReceived(
                [this](const auto& message) { m_dataReceiver->receiveData(message->buffer()); },
                guard());
        }
        sslClient.send(m_buffer);
        exec();
    }

private:
    SocketAddress m_address;
    SslData m_sslData;
    QByteArray m_buffer;
    DataReceiver* m_dataReceiver;
};

TEST(SslTransportTest, createClientGood)
{
    QJsonObject json = {
        {"mode", "client"},
        {"host", "localhost"},
        {"port", 12001},
        {"private_key", QString::fromStdString(appkit::configFile("key.pem"))},
        {"certificate", QString::fromStdString(appkit::configFile("cert.pem"))},
        {"ca_certificate", QString::fromStdString(appkit::configFile("ca_cert.pem"))}};
    std::unique_ptr<Transport> transport;
    ASSERT_NO_THROW(transport = createSslTransport(json));
    EXPECT_NE(nullptr, dynamic_cast<SslClient*>(transport.get()));
}

TEST(SslTransportTest, createSingleServerGood)
{
    QJsonObject json = {
        {"mode", "single_server"},
        {"host", "localhost"},
        {"port", 12002},
        {"private_key", QString::fromStdString(appkit::configFile("key.pem"))},
        {"certificate", QString::fromStdString(appkit::configFile("cert.pem"))},
        {"ca_certificate", QString::fromStdString(appkit::configFile("ca_cert.pem"))}};
    std::unique_ptr<Transport> transport;
    ASSERT_NO_THROW(transport = createSslTransport(json));
    EXPECT_NE(nullptr, dynamic_cast<SslSingleServer*>(transport.get()));
}

TEST(SslTransportTest, createMultiServerGood)
{
    QJsonObject json = {
        {"mode", "multi_server"},
        {"host", "localhost"},
        {"port", 12003},
        {"private_key", QString::fromStdString(appkit::configFile("key.pem"))},
        {"certificate", QString::fromStdString(appkit::configFile("cert.pem"))},
        {"ca_certificate", QString::fromStdString(appkit::configFile("ca_cert.pem"))}};
    std::unique_ptr<Transport> transport;
    ASSERT_NO_THROW(transport = createSslTransport(json));
    EXPECT_NE(nullptr, dynamic_cast<SslMultiServer*>(transport.get()));
}

TEST(SslTransportTest, createBad)
{
    QJsonObject json = {
        {"monde", "client"},
        {"houst", "localhost"},
        {"pourt", 12004},
        {"private_studio", QString::fromStdString(appkit::configFile("key.pem"))},
        {"cert", QString::fromStdString(appkit::configFile("cert.pem"))},
        {"ca_certifiooote", QString::fromStdString(appkit::configFile("ca_cert.pem"))}};
    EXPECT_THROW(createSslTransport(json), std::exception);
}

TEST(SslTransportTest, createMissedMode)
{
    QJsonObject json = {
        {"host", "localhost"},
        {"port", 12005},
        {"private_key", QString::fromStdString(appkit::configFile("key.pem"))},
        {"certificate", QString::fromStdString(appkit::configFile("cert.pem"))},
        {"ca_certificate", QString::fromStdString(appkit::configFile("ca_cert.pem"))}};
    EXPECT_THROW(createSslTransport(json), std::exception);
}

TEST(SslTransportTest, createMissedHost)
{
    QJsonObject json = {
        {"mode", "client"},
        {"port", 12006},
        {"private_key", QString::fromStdString(appkit::configFile("key.pem"))},
        {"certificate", QString::fromStdString(appkit::configFile("cert.pem"))},
        {"ca_certificate", QString::fromStdString(appkit::configFile("ca_cert.pem"))}};
    EXPECT_THROW(createSslTransport(json), std::exception);
}

TEST(SslTransportTest, createMissedPort)
{
    QJsonObject json = {
        {"mode", "client"},
        {"host", "localhost"},
        {"private_key", QString::fromStdString(appkit::configFile("key.pem"))},
        {"certificate", QString::fromStdString(appkit::configFile("cert.pem"))},
        {"ca_certificate", QString::fromStdString(appkit::configFile("ca_cert.pem"))}};
    EXPECT_THROW(createSslTransport(json), std::exception);
}

TEST(SslTransportTest, createMissedPrivateKey)
{
    QJsonObject json = {
        {"mode", "atata"},
        {"host", "localhost"},
        {"port", 12007},
        {"certificate", QString::fromStdString(appkit::configFile("cert.pem"))},
        {"ca_certificate", QString::fromStdString(appkit::configFile("ca_cert.pem"))}};
    EXPECT_THROW(createSslTransport(json), std::exception);
}

TEST(SslTransportTest, createMissedCert)
{
    QJsonObject json = {
        {"mode", "atata"},
        {"host", "localhost"},
        {"port", 12008},
        {"private_key", QString::fromStdString(appkit::configFile("key.pem"))},
        {"ca_certificate", QString::fromStdString(appkit::configFile("ca_cert.pem"))}};
    EXPECT_THROW(createSslTransport(json), std::exception);
}

TEST(SslTransportTest, createMissedCaCert)
{
    QJsonObject json = {
        {"mode", "atata"},
        {"host", "localhost"},
        {"port", 11011},
        {"private_key", QString::fromStdString(appkit::configFile("key.pem"))},
        {"certificate", QString::fromStdString(appkit::configFile("cert.pem"))}};
    EXPECT_THROW(createSslTransport(json), std::exception);
}

TEST(SslTransportTest, createInvalidMode)
{
    QJsonObject json = {
        {"mode", "atata"},
        {"host", "localhost"},
        {"port", 12009},
        {"private_key", QString::fromStdString(appkit::configFile("key.pem"))},
        {"certificate", QString::fromStdString(appkit::configFile("cert.pem"))},
        {"ca_certificate", QString::fromStdString(appkit::configFile("ca_cert.pem"))}};
    EXPECT_THROW(createSslTransport(json), std::exception);
}

TEST(SslTransportTest, singleServerSendNotConnected)
{
    SslSingleServer sslServer({QHostAddress::LocalHost, 12011}, sslData());
    EXPECT_THROW(sslServer.send("12345"), exception::TransportNotConnected);
}

TEST(SslTransportTest, multiServerSend)
{
    SslMultiServer sslServer({QHostAddress::LocalHost, 12012}, sslData());
    EXPECT_THROW(sslServer.send("12345"), exception::TransportMethodNotSupported);
}

template <typename Server>
class SslServerTest : public testing::Test
{
protected:
    std::unique_ptr<Server> m_server;
};

TYPED_TEST_SUITE_P(SslServerTest);

TYPED_TEST_P(SslServerTest, serverReceive)
{
    gqtest::ApplicationFixture app;

    auto address = SocketAddress{QHostAddress::LocalHost, 12013};
    QByteArray buffer("1234567890");

    ClientThread client(address, sslData(), buffer);

    DataReceiver dataReceiver;
    this->m_server = std::make_unique<TypeParam>(address, sslData());
    boostsignals::SignalTracker guard;
    this->m_server->onReceived(
        [&dataReceiver](const auto& message) { dataReceiver.receiveData(message->buffer()); },
        guard());

    EXPECT_CALL(dataReceiver, received(Eq(buffer))).Times(testing::Exactly(1));
    client.start();

    app.exec(&dataReceiver, &DataReceiver::dataReceived, 1000);
    client.quit();
    client.wait();
}

TYPED_TEST_P(SslServerTest, serverReply)
{
    gqtest::ApplicationFixture app;

    auto address = SocketAddress{QHostAddress::LocalHost, 12014};
    QByteArray buffer("1234567890");

    DataReceiver dataReceiver;
    ClientThread client(address, sslData(), buffer, &dataReceiver);

    boostsignals::SignalTracker guard;
    this->m_server = std::make_unique<TypeParam>(address, sslData());
    this->m_server->onReceived(
        [](const auto& message) { message->reply(message->buffer().repeated(2)); }, guard());

    EXPECT_CALL(dataReceiver, received(Eq(buffer.repeated(2)))).Times(testing::Exactly(1));
    client.start();

    app.exec(&dataReceiver, &DataReceiver::dataReceived, 1000);
    client.quit();
    client.wait();
}

REGISTER_TYPED_TEST_SUITE_P(SslServerTest, serverReceive, serverReply);

using SyncTypes = ::testing::Types<SslSingleServer, SslMultiServer>;
INSTANTIATE_TYPED_TEST_SUITE_P(My, SslServerTest, SyncTypes);

} // namespace rpc::tests
