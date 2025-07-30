#pragma once

#include <QObject>

namespace mpk
{

namespace storage_adapter
{

// Signals from this class intended to be connected by Qt::DirectConnection
// only. If connected through Qt::QueuedConnection, than at least
// ProviderSignals::aboutToRemove will hold index that is wrong or even
// out of range. Use it on your own risk.
class ProviderSignals : public QObject
{
    Q_OBJECT

public:
    explicit ProviderSignals(QObject* parent = nullptr) : QObject{parent}
    {
    }

signals:
    void aboutToAppend();
    void appendCompleted(int index);

    void changed(int index);

    void aboutToRemove(int index);
    void removeCompleted();

    void aboutToReset();
    void resetCompleted();

    void editingFinished();
};

} // namespace storage_adapter

} // namespace mpk
