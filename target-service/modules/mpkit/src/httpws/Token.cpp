#include "httpws/Token.h"

#include <QCryptographicHash>
#include <QJsonObject>

namespace http
{

namespace
{

constexpr auto valueTag = "value";
constexpr auto secsFromEpochTag = "secs_from_epoch";

QString generateToken()
{
    auto uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
    QCryptographicHash encoder(QCryptographicHash::Sha1);
#if QT_MAJOR < 6
    encoder.addData(uuid.toLocal8Bit(), uuid.size());
#else
    encoder.addData(QByteArrayView{uuid.toLocal8Bit()});
#endif
    const QByteArray encodeToken = encoder.result();
    return encodeToken.toHex();
}

} // namespace

Token Token::generate(Duration lifetime)
{
    return {generateToken(), Clock::now() + lifetime};
}

bool testToken(const Token& token)
{
    return token.expirationPoint > Token::Clock::now();
}

void resetToken(Token& token)
{
    token.expirationPoint = Token::Clock::now();
    token.value.clear();
}

bool operator==(const Token& lhs, const Token& rhs)
{
    return std::tie(lhs.expirationPoint, lhs.value) == std::tie(rhs.expirationPoint, rhs.value);
}

bool operator!=(const Token& lhs, const Token& rhs)
{
    return !(lhs == rhs);
}

} // namespace http

using namespace http;

Token FromJsonConverter<Token>::get(const QJsonValue& json)
{
    auto object = json::fromValue<QJsonObject>(json);
    auto value = json::fromObject<QString>(object, valueTag);
    auto secsFromEpoch = json::fromObject<int>(object, secsFromEpochTag);
    auto timePoint = Token::TimePoint() + std::chrono::seconds(secsFromEpoch);
    return Token{value, timePoint};
}

QJsonValue ToJsonConverter<Token>::convert(const Token& token)
{
    return QJsonObject{
        {valueTag, json::toValue(token.value)},
        {secsFromEpochTag,
         json::toValue(static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(
                                            token.expirationPoint.time_since_epoch())
                                            .count()))}};
}
