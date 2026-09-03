#ifndef RESXMLREADER_H
#define RESXMLREADER_H

#include "rsrescore_global.h"
#include <QStringList>

class ResPanel;
class QIODevice;
class QXmlStreamReader;
class LbrObjectInterface;
class ErrorsModel;
class ResBuffer;
class RSRESCORE_EXPORT ResXmlReader
{
public:
    ResXmlReader();
    virtual ~ResXmlReader();

    void readXml(QIODevice *device, bool validate = false); //throw(std::runtime_error, std::logic_error);
    void loadXml(LbrObjectInterface *pLbrObj, QIODevice *device, ErrorsModel *pErrorsModel = nullptr, bool validate = false);

    // Какие корневые теги <reslib> обрабатывает этот читатель
    // ("panel", "menu", ...). Используется ResXmlLoaderChain для
    // поэлементной диспетчеризации смешанных файлов
    virtual QStringList xmlTags() const;

protected:
    virtual void OnResRead(QXmlStreamReader *reader);
    //virtual void PrepareBuffer(QXmlStreamReader *reader, ResBuffer **resBuffer, ErrorsModel *pErrorsModel);

    // false — элемент не обработан (пропущен самим читателем через
    // skipCurrentElement): loadXml не пытается его сохранять. Если читатель
    // принял элемент, но разбор не удался — вернуть true, а ошибку
    // отрапортовать из SaveXmlToBuffer
    virtual bool BeginLoadXml(QXmlStreamReader *reader, QString &name, qint16 &type);
    virtual bool SaveXmlToBuffer(ResBuffer *resBuffer);
    virtual void EndLoadXml();

    friend class ResXmlLoaderChain;
};

#endif // RESXMLREADER_H
