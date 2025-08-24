#ifndef RESXMLLOADER_H
#define RESXMLLOADER_H

#include <resxmlreader.h>

class ErrorsModel;
class LbrObjectInterface;
class ResXmlLoader : public ResXmlReader
{
public:
    ResXmlLoader(LbrObjectInterface *pLbrObj, ErrorsModel *pErrorsModel = nullptr);

protected:
    virtual void OnResPanelReaded(ResPanel *panel);

private:
    LbrObjectInterface *m_pLbrObj;
    ErrorsModel* m_pErrorsModel;
};

#endif // RESXMLLOADER_H
