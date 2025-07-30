/** @file
 * @brief Application fixture class
 *
 * $Id: $
 */

#include "ApplicationFixture.h"

#include <QTimer>

#if __cplusplus < 201402L
#ifndef OS_WIN32 // weird check

namespace std
{

template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

} // namespace std

#endif
#endif

namespace gqtest
{

namespace
{

int argc = 1;
char arg1[] = "gtests";         // NOLINT
char* argv[] = {arg1, nullptr}; // NOLINT

} // namespace

ApplicationFixture::ApplicationFixture() :
  m_app(std::make_unique<QCoreApplication>(argc, argv))
{
}

void ApplicationFixture::wait(int intervalMsecs)
{
    if (intervalMsecs > 0)
    {
        QTimer::singleShot(intervalMsecs, m_app.get(), [this]() {
            m_timedOut = true;
            m_app->quit();
        });
    }
    m_app->exec();
}

void ApplicationFixture::exec(
    QObject* sender, const char* asignal, int intervalMsecs)
{
    Q_ASSERT(sender);
    QObject::connect(sender, asignal, m_app.get(), SLOT(quit()));
    wait(intervalMsecs);
}

bool ApplicationFixture::isTimedOut() const
{
    return m_timedOut;
}

} // namespace gqtest
