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
    return QList<qint16>()
           << LbrObject::RES_PANEL
           << LbrObject::RES_BS
           << LbrObject::RES_SCROL
           << LbrObject::RES_LS;
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
        "{c01bd070-a483-482c-9a30-2946a4317b71}",
        "{80384143-d159-4264-95c8-19e8a2cf7b70}"
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

        LbrObjectInterface *obj = nullptr;
        CreateLbrObject(&obj);

        QScopedPointer<LbrObjectInterface> tmp(obj);
        QFileInfo fi(filename);

        if (fi.completeSuffix().isEmpty())
            filename += ".lbr";

        tmp->create(filename);
    }
    else if (guid == "{57f88805-7474-42fb-bc00-24a90cd5e85d}")
    {
        // Панель (двойная рамка)
        pNewEditor = LoadResFromXmlTemplate(":/templates/EMPTY_PANEL_DOUBLE.xml",
                                            name,
                                            LbrObject::RES_PANEL);
    }
    else if (guid == "{c01bd070-a483-482c-9a30-2946a4317b71}")
    {
        // Панель из текста
    }
    else if (guid == "{80384143-d159-4264-95c8-19e8a2cf7b70}")
    {
        // Скролинг BSCROL (двойная рамка)
        pNewEditor = LoadResFromXmlTemplate(":/templates/EMPTY_BSCROL_DOUBLE.xml",
                                            name,
                                            LbrObject::RES_BS);
    }
    return pNewEditor;
}

BaseEditorWindow *BaseResourceEditor::editor(const qint16 &Type, const QString &name, LbrObjectInterface *pLbrObj)
{
    BaseEditorWindow *wnd = nullptr;

    static const QList<quint16> Types =
    {
        LbrObject::RES_PANEL,
        LbrObject::RES_BS,
        LbrObject::RES_SCROL,
        LbrObject::RES_LS
    };

    if (Types.contains(Type))
    {
        wnd = new StdPanelEditor(Type);
        wnd->setWindowIcon(RsResCore::inst()->iconFromResType(Type));
        wnd->setupEditor();

        ResBuffer *buffer = nullptr;
        pLbrObj->getResource(name, Type, &buffer);

        if (buffer)
        {
            QScopedPointer<ResPanel> testPan(new ResPanel());
            testPan->load(buffer);
            qobject_cast<StdPanelEditor*>(wnd)->setPanel(testPan.data(), buffer->comment());
            delete buffer;

            SetupEditorTitle(wnd, Type, name, testPan->title());
        }
        else
        {
            delete wnd;
            wnd = nullptr;
        }
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

BaseEditorWindow *BaseResourceEditor::LoadResFromXmlTemplate(const QString &filename,
                                                             const QString &name,
                                                             const quint16 &type)
{
    BaseEditorWindow *pNewEditor = nullptr;

    ResPanel *testPan = nullptr;
    QFile resxml(filename);
    if (!resxml.open(QIODevice::ReadOnly))
        return nullptr;

    RsResCore::inst()->loadFromXml(&resxml, &testPan);
    if (testPan)
    {
        pNewEditor = new StdPanelEditor(type);
        pNewEditor->setWindowIcon(RsResCore::inst()->iconFromResType(LbrObject::RES_PANEL));
        pNewEditor->setupEditor();

        testPan->setName(name);
        testPan->setType(type);
        qobject_cast<StdPanelEditor*>(pNewEditor)->setPanel(testPan);
        SetupEditorTitle(pNewEditor, type, name, testPan->title());
    }

    return pNewEditor;
}
