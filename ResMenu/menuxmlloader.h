#ifndef MENUXMLLOADER_H
#define MENUXMLLOADER_H

#include "ResMenu_global.h"
#include <QScopedPointer>
#include <QString>
#include <resxmlreader.h>

class EditorMenuBar;
class QXmlStreamReader;

// Импортер ресурсов меню из XML (элемент <menu> внутри <reslib>) —
// аналог ResXmlLoader для панелей. Разбор XML и запись в буфер
// выполняются через offscreen EditorMenuBar (setMenuFromXml/writeMenu) —
// теми же функциями, что и редактор, поэтому форматы не расходятся.
//
// Ожидаемый формат файла — как у MenuEditorWindow::saveToXml:
//   <reslib ...>
//     <menu name="..." dt="..." comment="..."> ... </menu>
//   </reslib>
class RESMENU_EXPORT MenuXmlLoader : public ResXmlReader
{
public:
    MenuXmlLoader();
    // в cpp: QScopedPointer<EditorMenuBar> требует полный тип
    // в точке инстанцирования деструктора
    virtual ~MenuXmlLoader();

    virtual QStringList xmlTags() const Q_DECL_OVERRIDE;

protected:
    void OnResRead(QXmlStreamReader *reader) Q_DECL_FINAL;

    bool BeginLoadXml(QXmlStreamReader *reader, QString &name, qint16 &type) Q_DECL_FINAL;
    bool SaveXmlToBuffer(ResBuffer *resBuffer) Q_DECL_FINAL;
    void EndLoadXml() Q_DECL_FINAL;

private:
    QScopedPointer<EditorMenuBar> m_menuBar; // offscreen-парсер текущего <menu>
    QString m_comment;
};

#endif // MENUXMLLOADER_H
