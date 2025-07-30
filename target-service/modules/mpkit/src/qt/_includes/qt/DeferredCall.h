#pragma once

#include <QObject>

#include <functional>
#include <memory>

class QTimer;

namespace qt
{

class DeferredCall : public QObject
{
    Q_OBJECT

public:
    using Call = std::function<void()>;
    DeferredCall(Call call, int timeout, QObject* parent = nullptr);
    DeferredCall(Call call, std::weak_ptr<bool> guard, int timeout, QObject* parent = nullptr);

public slots:
    void run();
    void runFree();

    static void asyncRun(Call call);
    static void asyncRun(Call call, std::weak_ptr<bool> guard);

private slots:
    void trigger();

private:
    Call m_call;
    std::shared_ptr<bool> m_guarded = nullptr;
    std::weak_ptr<bool> m_guard;
    QTimer* m_timer;
    bool m_suicide = false;
};

} // namespace qt
