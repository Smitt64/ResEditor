#ifndef RESXMLLOADER_H
#define RESXMLLOADER_H

#include <QScopedPointer>
#include <resxmlreader.h>

class ResPanel;
class ErrorsModel;
class LbrObjectInterface;
class QXmlStreamReader;
class ResXmlLoader : public ResXmlReader
{
public:
    ResXmlLoader(LbrObjectInterface *pLbrObj, ErrorsModel *pErrorsModel = nullptr);

    virtual QStringList xmlTags() const Q_DECL_OVERRIDE;

protected:
    void OnResRead(QXmlStreamReader *reader) Q_DECL_FINAL;

    virtual bool BeginLoadXml(QXmlStreamReader *reader, QString &name, qint16 &type) Q_DECL_FINAL;
    virtual bool SaveXmlToBuffer(ResBuffer *resBuffer) Q_DECL_FINAL;
    virtual void EndLoadXml() Q_DECL_FINAL;

private:
    LbrObjectInterface *m_pLbrObj;
    ErrorsModel* m_pErrorsModel;

    QScopedPointer<ResPanel> currentPanel;
    bool m_BeginOk;
};

#endif // RESXMLLOADER_H
