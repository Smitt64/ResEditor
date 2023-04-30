#include "baseresourceeditor.h"
#include "stdpaneleditor.h"
#include "respanel.h"
#include "rsrescore.h"
#include <lbrobject.h>
#include <resbuffer.h>
#include <QFile>
#include <QDir>
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
        "{57f88805-7474-42fb-bc00-24a90cd5e85d}"
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

        tmp->create(filename);
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

        wnd->setWindowTitle(QString("%1 [%2]: %3")
                            .arg(RsResCore::inst()->typeNameFromResType(Type))
                            .arg(name)
                            .arg(testPan->title()));
    }
    return wnd;
}
