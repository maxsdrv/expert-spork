#include "rpc/transport/Messenger.h"
#include "rpc/transport/IncomingMessageImpl.h"

#include <gmock/gmock.h>

#include <memory>

namespace rpc
{

class MessengerMock: public Messenger
{
public:
    MOCK_METHOD(void, send, (const QByteArray&), (noexcept, override));
};

class IncomingMessageImplTest : public ::testing::Test
{
public:
    IncomingMessageImplTest():
        m_array("1234567890"),
        m_messenger(std::make_shared<MessengerMock>()),
        m_incomingMessage(
            std::make_unique<IncomingMessageImpl>(m_array, m_messenger))
    {}

protected:
    QByteArray m_array;
    std::shared_ptr<MessengerMock> m_messenger;
    std::unique_ptr<IncomingMessage> m_incomingMessage;
};

using ::testing::Eq;

TEST_F(IncomingMessageImplTest, buffer)
{
    EXPECT_EQ(m_array, m_incomingMessage->buffer());
}

TEST_F(IncomingMessageImplTest, send)
{
    QByteArray data("0987654321");
    EXPECT_CALL(*m_messenger, send(Eq(data))).Times(testing::Exactly(1));
    m_incomingMessage->reply(data);
}

} // rpc
