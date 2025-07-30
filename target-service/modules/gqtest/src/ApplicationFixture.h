/** @file
 * @brief Application fixture class
 *
 * $Id: $
 */

#pragma once

#include <QCoreApplication>
#include <QTimer>

#include <memory>

namespace gqtest
{

/**
 * Class for providing event loop for tests
 */
class ApplicationFixture
{
public:
    ApplicationFixture();

    /**
     * Execute event loop for @a intervalMsecs milliseconds
     */
    void wait(int intervalMsecs);

    /**
     * Execute event loop until @a sender emits @a asignal.
     * Start timer if @a intervalMsecs is greater than zero.
     */
    void exec(QObject* sender, const char* asignal, int intervalMsecs = 0);
    /**
     * Execute event loop until @a sender emits @a asignal.
     * Start timer if @a intervalMsecs is greater than zero.
     */
    template <typename Signal>
    void exec(
        const typename QtPrivate::FunctionPointer<Signal>::Object* sender,
        Signal signal,
        int intervalMsecs = 0)
    {
        Q_ASSERT(sender);
        QObject::connect(sender, signal, m_app.get(), &QCoreApplication::quit);
        wait(intervalMsecs);
    }

    /**
     * Return true if event loop was quited by timer signal
     */
    bool isTimedOut() const;

private:
    std::unique_ptr<QCoreApplication> m_app;
    bool m_timedOut = false;
};

} // namespace gqtest
