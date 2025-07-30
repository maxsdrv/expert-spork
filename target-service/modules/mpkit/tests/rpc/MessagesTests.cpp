#include "rpc/transport/messages/HeaderMessageWrapper.h"

#include <gmock/gmock.h>

#include <memory>

namespace rpc
{

template <typename T>
class HeaderTest : public testing::Test
{
public:
    HeaderTest():
        m_header(0xFFFF)
    {}

protected:
    SyncChecksumLengthBEHeader<quint16, quint16> m_header;
};

TYPED_TEST_SUITE_P(HeaderTest);

TYPED_TEST_P(HeaderTest, singleMessage)
{
    QByteArray message("abcdefghijklmnopqrstuvwxyz");
    auto [result, remainder] =
        this->m_header.singleUnwrap(this->m_header.singleWrap(message));

    EXPECT_EQ(message, result);
    EXPECT_TRUE(remainder.isEmpty());
}

TYPED_TEST_P(HeaderTest, singleMessageWithHeadGarbage)
{
    QByteArray message("abcdefghijklmnopqrstuvwxyz");

    auto wrapped = this->m_header.singleWrap(message);
    auto garbage = QByteArray("\0\0\1\2\3\4\5\6zzxxff", 14);
    auto [result, remainder] = this->m_header.singleUnwrap(garbage + wrapped);

    EXPECT_EQ(message, result);
    EXPECT_TRUE(remainder.isEmpty());
}

TYPED_TEST_P(HeaderTest, singleMessageWithTailGarbage)
{
    QByteArray message("abcdefghijklmnopqrstuvwxyz");

    auto wrapped = this->m_header.singleWrap(message);
    auto garbage = QByteArray("\0\0\1\2\3\4\5\6zzxxff", 14);
    auto [result, remainder] = this->m_header.singleUnwrap(wrapped + garbage);

    EXPECT_EQ(message, result);
    EXPECT_EQ(garbage, remainder);
}

TYPED_TEST_P(HeaderTest, singleMessageWithBothGarbage)
{
    QByteArray message("abcdefghijklmnopqrstuvwxyz");

    auto wrapped = this->m_header.singleWrap(message);
    auto garbage = QByteArray("\0\0\1\2\3\4\5\6zzxxff", 14);
    auto moreGarbage = QByteArray("\7\6\5\4\3\2ddffggee", 14);
    auto [result, remainder] =
        this->m_header.singleUnwrap(garbage + wrapped + moreGarbage);

    EXPECT_EQ(message, result);
    EXPECT_EQ(moreGarbage, remainder);
}

REGISTER_TYPED_TEST_SUITE_P(
    HeaderTest,
    singleMessage,
    singleMessageWithHeadGarbage,
    singleMessageWithTailGarbage,
    singleMessageWithBothGarbage);

using SyncTypes = ::testing::Types<quint16, quint32, quint64>;
INSTANTIATE_TYPED_TEST_SUITE_P(My, HeaderTest, SyncTypes);


class WrapperTest: public testing::Test
{
public:
    WrapperTest():
        m_wrapper(0xFFFF)
    {}

protected:
    HeaderMessageWrapper<SyncChecksumLengthBEHeader<quint16, quint16>> m_wrapper;
};


TEST_F(WrapperTest, singleMessage)
{
    QByteArray message("abcdefghijklmnopqrstuvwxyz");
    auto result = m_wrapper.unwrap(m_wrapper.wrap(message));

    ASSERT_EQ(1, result.size());
    EXPECT_EQ(message, result.front());
}

TEST_F(WrapperTest, singleMessage2Chunks)
{
    QByteArray message("abcdefghijklmnopqrstuvwxyz");
    auto wrapped = m_wrapper.wrap(message);
    auto result1 = m_wrapper.unwrap(wrapped.left(25));
    auto result2 = m_wrapper.unwrap(wrapped.mid(25));

    ASSERT_EQ(0, result1.size());
    ASSERT_EQ(1, result2.size());
    EXPECT_EQ(message, result2.front());
}

TEST_F(WrapperTest, singleMessageLessThanHeader)
{
    QByteArray message("abcdefghijklmnopqrstuvwxyz");
    auto wrapped = m_wrapper.wrap(message);
    auto result1 = m_wrapper.unwrap(wrapped.left(5));
    auto result2 = m_wrapper.unwrap(wrapped.mid(5));

    ASSERT_EQ(0, result1.size());
    ASSERT_EQ(1, result2.size());
    EXPECT_EQ(message, result2.front());
}

TEST_F(WrapperTest, twoMessages)
{
    QByteArray message1("abcdefghijklmnopqrstuvwxyz");
    QByteArray message2("zyxwvutsrqponmlkjihgfedcba");
    auto wrapped1 = m_wrapper.wrap(message1);
    auto wrapped2 = m_wrapper.wrap(message2);

    auto result = m_wrapper.unwrap(wrapped1 + wrapped2);

    ASSERT_EQ(2, result.size());
    EXPECT_EQ(message1, result.front());
    EXPECT_EQ(message2, result.back());
}

TEST_F(WrapperTest, shredder)
{
    QByteArray message1("abcdefghijklmnopqrstuvwxyz");
    QByteArray message2("zyxwvutsrqponmlkjihgfedcba");
    auto wrapped1 = m_wrapper.wrap(message1);
    auto wrapped2 = m_wrapper.wrap(message2);

    auto splitted = wrapped1 + wrapped2;
    std::vector<QByteArray> result;
    for(auto x=0; x< splitted.size(); ++x)
    {
        auto res = m_wrapper.unwrap(splitted.mid(x, 1));
        if(!res.empty())
        {
            result.push_back(res.front());
        }
    }

    ASSERT_EQ(2, result.size());
    EXPECT_EQ(message1, result.front());
    EXPECT_EQ(message2, result.back());
}

} // rpc
