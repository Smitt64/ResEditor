#ifndef RESXMLREADER_H
#define RESXMLREADER_H

#include <stdexcept>
#include "rsrescore_global.h"

class ResPanel;
class QIODevice;
class RSRESCORE_EXPORT ResXmlReader
{
public:
    ResXmlReader();
    ~ResXmlReader();

    void readXml(QIODevice *device, bool validate = false); //throw(std::runtime_error, std::logic_error);

protected:
    virtual void OnResPanelReaded(ResPanel *res);
};

#endif // RESXMLREADER_H
