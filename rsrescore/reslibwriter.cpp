#include "reslibwriter.h"
#include "respanel.h"

ResLibWriter::ResLibWriter() :
    m_isStarted(false)
{

}

ResLibWriter::~ResLibWriter()
{
    if (m_isStarted)
        end();
}

bool ResLibWriter::begin(const QString &filename, const QString &encode)
{
    if (m_isStarted) {
        m_errorString = "Writer already started";
        return false;
    }

    m_file.setFileName(filename);
    if (!m_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_errorString = QString("Cannot open file: %1").arg(m_file.errorString());
        return false;
    }

    m_writer.setDevice(&m_file);
    m_writer.setAutoFormatting(true);
    m_writer.setAutoFormattingIndent(2);

    if (!encode.isEmpty()) {
        m_writer.writeStartDocument();
        m_writer.writeDefaultNamespace("http://www.softlab.ru");
    } else {
        m_writer.writeStartDocument("1.0");
    }

    // Пишем корневой элемент с namespace
    m_writer.writeStartElement("reslib");
    m_writer.writeAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
    m_writer.writeAttribute("xsi:schemaLocation", "http://www.softlab.ru reslib.xsd");
    m_writer.writeAttribute("xmlns", "http://www.softlab.ru");

    m_isStarted = true;
    return true;
}

bool ResLibWriter::end()
{
    if (!m_isStarted) {
        m_errorString = "Writer not started";
        return false;
    }

    try {
        m_writer.writeEndElement(); // reslib
        m_writer.writeEndDocument();
        m_file.close();
        m_isStarted = false;
        return true;
    } catch (...) {
        m_errorString = "Error writing document end";
        m_file.close();
        m_isStarted = false;
        return false;
    }
}

bool ResLibWriter::addResource(ResPanel *panel)
{
    if (!m_isStarted) {
        m_errorString = "Writer not started";
        return false;
    }

    if (!panel) {
        m_errorString = "Null panel pointer";
        return false;
    }

    try {
        return panel->saveToXml(m_writer);
    } catch (...) {
        m_errorString = "Error writing panel to XML";
        return false;
    }
}

QString ResLibWriter::errorString() const
{
    return m_errorString;
}

bool ResLibWriter::hasError() const
{
    return !m_errorString.isEmpty();
}

QXmlStreamWriter &ResLibWriter::writer()
{
    return m_writer;
}
