#include "resxmlreader.h"
#include <QXmlStreamReader>
#include <QIODevice>
#include "errorsmodel.h"
#include "lbrobjectinterface.h"
#include "qapplication.h"
#include "respanel.h"
#include "rsrescore.h"

ResXmlReader::ResXmlReader()
{

}

ResXmlReader::~ResXmlReader()
{

}

void ResXmlReader::readXml(QIODevice *device, bool validate) // throw(std::runtime_error, std::logic_error)
{
    if (validate && !RsResCore::inst()->validateResXmlWithXsd(device))
        throw std::runtime_error("XML validation failed against XSD schema");

    device->seek(0);

    QXmlStreamReader reader(device);
    while (!reader.atEnd() && !reader.hasError())
    {
        QXmlStreamReader::TokenType token = reader.readNext();

        if (token == QXmlStreamReader::StartElement)
        {
            if (reader.name() == "reslib")
                continue;

            OnResRead(&reader);
        }
    }

    if (reader.hasError())
        throw std::runtime_error(reader.errorString().toStdString());
}

void ResXmlReader::loadXml(LbrObjectInterface *pLbrObj, QIODevice *device, ErrorsModel *pErrorsModel, bool validate)
{
    if (validate && !RsResCore::inst()->validateResXmlWithXsd(device))
        throw std::runtime_error("XML validation failed against XSD schema");

    device->seek(0);

    QXmlStreamReader reader(device);
    while (!reader.atEnd() && !reader.hasError())
    {
        QXmlStreamReader::TokenType token = reader.readNext();

        if (token == QXmlStreamReader::StartElement)
        {
            if (reader.name() == "reslib")
                continue;

            bool exists = false;
            bool deleted = true;
            ResBuffer *resBuffer = nullptr;

            QString name; qint16 type;
            // false — читатель не обработал элемент (неизвестный тег уже
            // пропущен через skipCurrentElement): в библиотеку не пишем
            if (!BeginLoadXml(&reader, name, type))
            {
                EndLoadXml();
                continue;
            }

            if (pLbrObj->isResExists(name, type))
            {
                QApplication::processEvents();
                exists = true;
                if (!pLbrObj->deleteResource(name, type))
                {
                    deleted = false;
                    if (pErrorsModel)
                        pErrorsModel->appendError(QObject::tr("Не удалось перезаписать ресурс <b>%1</b>").arg(name));
                }
            }

            if (deleted)
            {
                QString errorMsg;
                if (pLbrObj->beginSaveRes(name, type, &resBuffer))
                {
                    if (SaveXmlToBuffer(resBuffer))
                    {
                        if (pErrorsModel)
                        {
                            if (!exists)
                                pErrorsModel->appendMessage(QObject::tr("Ресурс <b>%1</b> успешно загружен").arg(name));
                            else
                                pErrorsModel->appendMessage(QObject::tr("Ресурс <b>%1</b> успешно перезаписан").arg(name));
                        }

                        pLbrObj->endSaveRes(&resBuffer);
                    }
                    else
                    {
                        if (pErrorsModel)
                            pErrorsModel->appendError(QObject::tr("Не удалось загрузить ресурс <b>%1</b>").arg(name));
                    }
                }
                else
                {
                    if (pErrorsModel)
                        pErrorsModel->appendError(QObject::tr("Не удалось загрузить ресурс <b>%1</b>").arg(name));
                }
            }

            EndLoadXml();
        }
    }

    QApplication::processEvents();

    if (reader.hasError())
        throw std::runtime_error(reader.errorString().toStdString());
}

void ResXmlReader::OnResRead(QXmlStreamReader *reader)
{

}

QStringList ResXmlReader::xmlTags() const
{
    return QStringList();
}

bool ResXmlReader::BeginLoadXml(QXmlStreamReader *reader, QString &name, qint16 &type)
{
    return false;
}

bool ResXmlReader::SaveXmlToBuffer(ResBuffer *resBuffer)
{
    return false;
}

void ResXmlReader::EndLoadXml()
{

}

/*void ResXmlReader::PrepareBuffer(QXmlStreamReader *reader, ResBuffer **resBuffer, ErrorsModel *pErrorsModel)
{
    QScopedPointer<ResPanel> currentPanel(new ResPanel());
    currentPanel->loadXmlStream(*reader);
}*/