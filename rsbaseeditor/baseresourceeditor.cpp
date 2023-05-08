#include "baseresourceeditor.h"
#include "stdpaneleditor.h"
#include "respanel.h"
#include "rsrescore.h"
#include <lbrobject.h>
#include <resbuffer.h>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
//Q_IMPORT_PLUGIN(BaseResourceEditor)

BaseResourceEditor::BaseResourceEditor() :
    QObject(nullptr),
    ResourceEditorInterface()
{

}

BaseResourceEditor::~BaseResourceEditor()
{

}

QList<qint16> BaseResourceEditor::resTypes() const
{
    return QList<qint16>() << LbrObject::RES_PANEL;
}

QString BaseResourceEditor::newItemsMetaList()
{
    QFile f(":/json/BaseResourceNewItems.json");
    f.open(QIODevice::ReadOnly);

    QTextStream stream(&f);
    stream.setCodec("UTF-8");
    return stream.readAll();
}

bool BaseResourceEditor::newItemsActionAvalible(const QString &guid)
{
    static QStringList actions
    {
        "{c7e4dbe9-cd8e-4eaf-bcd3-975f9fb6ba1e}",
        "{57f88805-7474-42fb-bc00-24a90cd5e85d}",
        "{c01bd070-a483-482c-9a30-2946a4317b71}"
    };

    return actions.contains(guid);
}

BaseEditorWindow *BaseResourceEditor::newItemsAction(const QString &guid, const QString &name, const QString &path)
{
    BaseEditorWindow *pNewEditor = nullptr;

    if (guid == "{c7e4dbe9-cd8e-4eaf-bcd3-975f9fb6ba1e}")
    {
        // Создание новой библиотеки ресурсов
        QDir dir(path);
        QString filename = dir.absoluteFilePath(name);
        QScopedPointer<LbrObject> tmp(new LbrObject());
        QFileInfo fi(filename);

        if (fi.completeSuffix().isEmpty())
            filename += ".lbr";

        tmp->create(filename);
    }
    else if (guid == "{57f88805-7474-42fb-bc00-24a90cd5e85d}")
    {
        // Панель (двойная рамка)
        ResPanel *testPan = nullptr;
        QFile resxml(":/templates/EMPTY_PANEL_DOUBLE.xml");
        if (!resxml.open(QIODevice::ReadOnly))
            return nullptr;

        RsResCore::inst()->loadFromXml(&resxml, &testPan);
        if (testPan)
        {
            pNewEditor = new StdPanelEditor();
            pNewEditor->setWindowIcon(RsResCore::inst()->iconFromResType(LbrObject::RES_PANEL));
            pNewEditor->setupEditor();

            testPan->setName(name);
            qobject_cast<StdPanelEditor*>(pNewEditor)->setPanel(testPan);
            SetupEditorTitle(pNewEditor, LbrObject::RES_PANEL, name, testPan->title());
        }
    }
    else if (guid == "{c01bd070-a483-482c-9a30-2946a4317b71}")
    {
        // Панель из текста
    }
    return pNewEditor;
}

BaseEditorWindow *BaseResourceEditor::editor(const qint16 &Type, const QString &name, LbrObject *pLbrObj)
{
    BaseEditorWindow *wnd = nullptr;

    if (Type == LbrObject::RES_PANEL)
    {
        wnd = new StdPanelEditor();
        wnd->setWindowIcon(RsResCore::inst()->iconFromResType(Type));
        wnd->setupEditor();

        ResBuffer *buffer = nullptr;
        pLbrObj->getResource(name, Type, &buffer);
        QScopedPointer<ResPanel> testPan(new ResPanel());
        testPan->load(buffer);
        qobject_cast<StdPanelEditor*>(wnd)->setPanel(testPan.data(), buffer->comment());
        delete buffer;

        SetupEditorTitle(wnd, Type, name, testPan->title());
    }
    return wnd;
}

void BaseResourceEditor::SetupEditorTitle(BaseEditorWindow *wnd, const qint16 &Type, const QString &name, const QString &title)
{
    wnd->setWindowTitle(QString("%1 [%2]: %3")
                        .arg(RsResCore::inst()->typeNameFromResType(Type))
                        .arg(name)
                        .arg(title));
}
