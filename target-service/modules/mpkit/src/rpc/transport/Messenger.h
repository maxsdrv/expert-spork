#pragma once

#include <QObject>

namespace rpc
{

class Messenger : public QObject
{
    Q_OBJECT

public:
    explicit Messenger(QObject* parent = nullptr) : QObject(parent)
    {
    }

    virtual void send(const QByteArray& array) noexcept = 0;

signals:
    void received(QByteArray);
};

} // namespace rpc
