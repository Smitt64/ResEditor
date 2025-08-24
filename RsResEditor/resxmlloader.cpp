#include "resxmlloader.h"
#include "lbrobjectinterface.h"
#include "respanel.h"
#include "errorsmodel.h"
#include <QObject>
#include <QApplication>

ResXmlLoader::ResXmlLoader(LbrObjectInterface *pLbrObj, ErrorsModel *pErrorsModel) :
    m_pLbrObj(pLbrObj),
    m_pErrorsModel(pErrorsModel)
{

}

void ResXmlLoader::OnResPanelReaded(ResPanel *panel)
{
    bool exists = false;
    bool deleted = true;
    if (m_pLbrObj->isResExists(panel->name(), panel->type()))
    {
        QApplication::processEvents();
        exists = true;
        if (!m_pLbrObj->deleteResource(panel->name(), panel->type()))
        {
            deleted = false;
            if (m_pErrorsModel)
                m_pErrorsModel->appendError(QObject::tr("Не удалось перезаписать ресурс <b>%1</b>").arg(panel->name()));
        }
    }

    if (deleted)
    {
        QString errorMsg;
        ResBuffer *resBuffer = nullptr;

        if (m_pLbrObj->beginSaveRes(panel->name(), panel->type(), &resBuffer))
        {
            if (!panel->save(resBuffer))
            {
                if (m_pErrorsModel)
                {
                    if (!exists)
                        m_pErrorsModel->appendMessage(QObject::tr("Ресурс <b>%1</b> успешно загружен").arg(panel->name()));
                    else
                        m_pErrorsModel->appendMessage(QObject::tr("Ресурс <b>%1</b> успешно перезаписан").arg(panel->name()));
                }

                m_pLbrObj->endSaveRes(&resBuffer);
            }
            else
            {
                if (m_pErrorsModel)
                    m_pErrorsModel->appendError(QObject::tr("Не удалось загрузить ресурс <b>%1</b>").arg(panel->name()));
            }
        }
        else
        {
            if (m_pErrorsModel)
                m_pErrorsModel->appendError(QObject::tr("Не удалось загрузить ресурс <b>%1</b>").arg(panel->name()));
        }
    }

    QApplication::processEvents();
}
