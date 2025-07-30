#include "rpc/transport/Socket.h"

#include "json/JsonUtilities.h"

#include <gmock/gmock.h>

namespace rpc
{

TEST(SocketAddressTest, fromJsonAny)
{
    auto json = json::fromByteArray("{ \"host\": \"any\", \"port\": 4545 }");
    SocketAddress socketAddr;
    ASSERT_NO_THROW(socketAddr = json::fromValue<SocketAddress>(json));
    EXPECT_EQ(QHostAddress::Any, socketAddr.host);
    EXPECT_EQ(4545, socketAddr.port);
}

TEST(SocketAddressTest, fromJsonLocalhost)
{
    auto json = json::fromByteArray("{ \"host\": \"localhost\", \"port\": 2345 }");
    SocketAddress socketAddr;
    ASSERT_NO_THROW(socketAddr = json::fromValue<SocketAddress>(json));
    EXPECT_EQ(QHostAddress::LocalHost, socketAddr.host);
    EXPECT_EQ(2345, socketAddr.port);
}

TEST(SocketAddressTest, fromJsonDotted)
{
    auto json = json::fromByteArray("{ \"host\": \"10.10.0.1\", \"port\": 4645 }");
    SocketAddress socketAddr;
    ASSERT_NO_THROW(socketAddr = json::fromValue<SocketAddress>(json));
    EXPECT_EQ("10.10.0.1", socketAddr.host.toString());
    EXPECT_EQ(4645, socketAddr.port);
}

TEST(SocketAddressTest, fromJsonAbsentHost)
{
    auto json = json::fromByteArray("{ \"port\": 4645 }");
    SocketAddress socketAddr;
    ASSERT_THROW(socketAddr = json::fromValue<SocketAddress>(json), std::exception);
}

TEST(SocketAddressTest, fromJsonAbsentPort)
{
    auto json = json::fromByteArray("{ \"host\": \"10.10.0.1\" }");
    SocketAddress socketAddr;
    ASSERT_THROW(socketAddr = json::fromValue<SocketAddress>(json), std::exception);
}

} // rpc
