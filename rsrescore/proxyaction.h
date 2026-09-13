#ifndef PROXYACTION_H
#define PROXYACTION_H

#include <QAction>
#include "rsrescore_global.h"

class RSRESCORE_EXPORT ProxyAction : public QAction
{
    Q_OBJECT
public:
    explicit ProxyAction(QObject *parent = nullptr);
    virtual ~ProxyAction();

    void setSource(QAction *source);

private:
    QAction *m_pSource;
    QMetaObject::Connection m_ChangeConnection, m_TriggerConnection;
};

#endif // PROXYACTION_H
