#pragma once

#include <QObject>

#include <gmock/gmock.h>

namespace rpc
{

/**
 * This class has complex strucure to:
 * signal "dataReceived" to connect with ApplicationFixture to stop event loop
 * when data is received
 * mocked function "received" to use it in EXPECT_CALL
 */
class DataReceiver : public QObject
{
    Q_OBJECT


public:
    MOCK_CONST_METHOD1(received, void(const QByteArray&));

public slots:
    void receiveData(const QByteArray& data)
    {
        received(data);
        emit dataReceived();
    }

signals:
    void dataReceived();
};

} // namespace rpc
