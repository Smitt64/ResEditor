#ifndef RESLIBWRITER_H
#define RESLIBWRITER_H

#include <QObject>
#include <QXmlStreamWriter>
#include <QFile>
#include "rsrescore_global.h"

class ResPanel;
class RSRESCORE_EXPORT ResLibWriter
{
public:
    ResLibWriter();
    virtual ~ResLibWriter();

    bool begin(const QString &filename, const QString &encode = "UTF-8");
    bool end();
    bool addResource(ResPanel *panel);

    QString errorString() const;
    bool hasError() const;

    QXmlStreamWriter &writer();

private:
    QFile m_file;
    QXmlStreamWriter m_writer;
    QString m_errorString;
    bool m_isStarted;
};

#endif // RESLIBWRITER_H
