#pragma once

#include <QObject>
#include <QString>

namespace mpk::dss::core
{

class ProviderSignals : public QObject
{
    Q_OBJECT

public:
    explicit ProviderSignals(QObject* parent = nullptr) : QObject(parent)
    {
    }

signals:
    void aboutToAppend();
    void appendCompleted(QString);

    void changed(QString);

    void aboutToRemove(QString);
    void removeCompleted();

    void aboutToReset();
    void resetCompleted();

    void modified();
};

} // namespace mpk::dss::core
