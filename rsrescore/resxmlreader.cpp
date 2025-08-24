#include "resxmlreader.h"
#include <QXmlStreamReader>
#include <QIODevice>
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
    static const QStringList RootTags =
    {
        "panel",
        "bscrol",
        "scrol",
        "lscrol"
    };

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

            if (RootTags.contains(reader.name().toString()))
            {
                QScopedPointer<ResPanel> currentPanel(new ResPanel());
                currentPanel->loadXmlStream(reader);
                OnResPanelReaded(currentPanel.data());
            }
        }
    }

    if (reader.hasError())
        throw std::runtime_error(reader.errorString().toStdString());
}

void ResXmlReader::OnResPanelReaded(ResPanel *res)
{

}
