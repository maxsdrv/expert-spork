/** @file
 * @brief Helper class to execute actions after event loop has been started
 *
 * @ingroup
 *
 *
 * $Id: $
 */

#include <QObject>

#include <functional>
#include <vector>

#pragma once

namespace gqtest
{

/**
 * Some actions should be done only in event loop.
 * This class simplify it.
 */
class DelayedExecution : public QObject
{
    Q_OBJECT
public:
    using Function = std::function<void()>;

signals:
    /**
     * The signal is emmited in the end of execution
     */
    void complete();

public:
    /**
     * Create DelayedExecution object
     */
    DelayedExecution();

    /**
     * Add action to execute
     */
    void addAction(Function action);

public slots:
    /**
     * Send complete signal. One can bind own qt or boost to this function
     */
    void sendComplete();

private slots:
    void execute();

private:
    std::vector<Function> m_actions;
};

} // namespace gqtest
