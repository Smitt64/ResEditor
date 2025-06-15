#include "proxyaction.h"

ProxyAction::ProxyAction(QObject *parent)
    : QAction{parent},
    m_pSource(nullptr)
{
    setEnabled(false);
}

ProxyAction::~ProxyAction()
{

}

void ProxyAction::setSource(QAction *source)
{
    if (m_pSource)
    {
        disconnect(m_ChangeConnection);
        disconnect(m_TriggerConnection);
        m_ChangeConnection = QMetaObject::Connection();
        m_TriggerConnection = QMetaObject::Connection();
        m_pSource = nullptr;
    }

    if (!source)
        return;

    m_pSource = source;
    setEnabled(m_pSource->isEnabled());

    m_ChangeConnection = connect(m_pSource, &QAction::changed, [=]()
    {
        setEnabled(m_pSource->isEnabled());
    });

    m_TriggerConnection = connect(this, &QAction::triggered, m_pSource, &QAction::trigger);
}
