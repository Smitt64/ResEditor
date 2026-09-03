#ifndef RESXMLLOADERCHAIN_H
#define RESXMLLOADERCHAIN_H

#include "rsrescore_global.h"
#include "resxmlreader.h"
#include <QList>

class ErrorsModel;
class QXmlStreamReader;

// Поэлементная диспетчеризация импорта reslib-XML: один файл может
// содержать ресурсы разных типов (<panel>, <bscrol>, <menu>, ...),
// каждый тип обрабатывает свой ResXmlReader (панели — в exe, меню и
// прочие — из плагинов через ResourceEditorInterface::xmlImporter).
// Цепочка выбирает обработчика по ResXmlReader::xmlTags().
//
// Импортерами цепочка НЕ владеет: время жизни должно покрывать вызов
// loadXml/readXml (плагинные импортеры живут до выгрузки плагинов).
class RSRESCORE_EXPORT ResXmlLoaderChain : public ResXmlReader
{
public:
    ResXmlLoaderChain(ErrorsModel *pErrorsModel = nullptr);

    void addImporter(ResXmlReader *importer);

protected:
    void OnResRead(QXmlStreamReader *reader) Q_DECL_OVERRIDE;

    bool BeginLoadXml(QXmlStreamReader *reader, QString &name, qint16 &type) Q_DECL_OVERRIDE;
    bool SaveXmlToBuffer(ResBuffer *resBuffer) Q_DECL_OVERRIDE;
    void EndLoadXml() Q_DECL_OVERRIDE;

private:
    ResXmlReader *importerForTag(const QString &tag) const;

    QList<ResXmlReader*> m_importers;
    ResXmlReader *m_current;
    ErrorsModel *m_pErrorsModel;
};

#endif // RESXMLLOADERCHAIN_H
