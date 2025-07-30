/** @file
 * @brief     State converter injection
 *
 * $Id: $
 */

#pragma once

#include <QObject>

namespace uistatesaver
{

/**
 * Base class for save/restore state of specific QObject subclass
 * One should subclass it to provide specific save/restore abilities.
 */
class StateConverterInjection : public QObject
{
    Q_OBJECT

public:
    /**
     * Create injection with @a id. @a id is a key for storing object state.
     */
    explicit StateConverterInjection(QString id, QObject* parent = nullptr);
    /**
     * Return injection id
     */
    QString id() const;
    /**
     * Return state converted to QByteArray (widget specific, saveState for
     * QSplitter for example)
     */
    virtual QByteArray state() const = 0;
    /**
     * Restore state from a past session
     */
    virtual void restore(const QByteArray& state) = 0;

signals:
    /**
     * Emit signal when state should be saved
     */
    void saveRequested(uistatesaver::StateConverterInjection*);
    /**
     * Emit signal when state should be restored
     */
    void restoreRequested(uistatesaver::StateConverterInjection*);

protected slots:
    void requestSave();
    void requestRestore();

private:
    QString m_id;
    bool m_wasRestored = false;
};

} // namespace uistatesaver
