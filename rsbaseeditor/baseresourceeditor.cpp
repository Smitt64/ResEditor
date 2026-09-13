#include "baseresourceeditor.h"
#include "stdpaneleditor.h"
#include "respanel.h"
#include "rsrescore.h"
#include "wizards/texttopanel/texttopanelwizard.h"
#include "panelitem.h"
#include <lbrobject.h>
#include <resbuffer.h>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <errorsmodel.h>
#include <errordlg.h>
#include <widgets/codeinputdialog.h>
#include "SARibbon.h"
//Q_IMPORT_PLUGIN(BaseResourceEditor)

BaseResourceEditor::BaseResourceEditor() :
    QObject(nullptr),
    ResourceEditorInterface()
{
    // Пользовательские шаблоны панелей: пользовательский каталог
    // в приоритете над рабочим и программным (добавлен первым)
    m_templates.addSearchDir(ResTemplateRegistry::userTemplatesDir(QStringLiteral("panels")));
    m_templates.addSearchDir(ResTemplateRegistry::workingTemplatesDir(QStringLiteral("panels")));
    m_templates.addSearchDir(ResTemplateRegistry::programTemplatesDir(QStringLiteral("panels")));
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

    // Диалог создания пересобирается каждый раз — перечитываем шаблоны,
    // подложенные файлы видны без перезапуска приложения
    m_templates.rescan();

    return m_templates.appendToMetaList(stream.readAll());
}

bool BaseResourceEditor::newItemsActionAvalible(const QString &guid)
{
    static QStringList actions
    {
        "{c7e4dbe9-cd8e-4eaf-bcd3-975f9fb6ba1e}",
        "{57f88805-7474-42fb-bc00-24a90cd5e85d}",
        "{c01bd070-a483-482c-9a30-2946a4317b71}",
        "{80384143-d159-4264-95c8-19e8a2cf7b70}",
        "{eaeac9f8-3230-4015-a029-ac337c3d83e9}",
        "{001b506e-4588-4810-a09a-d631fc0214d8}"
    };

    return actions.contains(guid) || m_templates.contains(guid);
}

void BaseResourceEditor::ShowErrors(ErrorsModel *model, QWidget *parent)
{
    ErrorDlg dlg(parent);
    dlg.setErrors(model);
    dlg.exec();
}

ResourceEditorResult BaseResourceEditor::newItemsAction(const QString &guid, const QString &name, const QString &path, QWidget *parent)
{
    ResourceEditorResult result;
    BaseEditorWindow *pNewEditor = nullptr;

    ErrorsModel errors;
    if (m_templates.contains(guid))
    {
        // Пользовательский шаблон из файла: тип ресурса определится
        // по корневому тегу xml при загрузке
        QFile tpl(m_templates.templatePath(guid));

        if (tpl.open(QIODevice::ReadOnly))
        {
            pNewEditor = LoadResFromXmlTemplate(&tpl, name,
                                                {LbrObject::RES_PANEL, LbrObject::RES_BS,
                                                 LbrObject::RES_SCROL, LbrObject::RES_LS},
                                                &errors);

            if (!pNewEditor)
                ShowErrors(&errors, parent);
        }
        else
        {
            errors.appendError(tr("Не удалось открыть файл шаблона <b>%1</b>")
                               .arg(m_templates.templatePath(guid)));
            ShowErrors(&errors, parent);
        }
    }
    else if (guid == "{c7e4dbe9-cd8e-4eaf-bcd3-975f9fb6ba1e}")
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

        result.succeed = tmp->create(filename);

        // Сообщаем главному окну путь к созданной библиотеке —
        // оно откроет её в приложении
        if (result.succeed)
            result.fileName = filename;
    }
    else if (guid == "{57f88805-7474-42fb-bc00-24a90cd5e85d}")
    {
        // Панель (двойная рамка)
        pNewEditor = LoadResFromXmlTemplate(":/templates/EMPTY_PANEL_DOUBLE.xml",
                                            name,
                                            LbrObject::RES_PANEL,
                                            &errors);

        if (!pNewEditor)
            ShowErrors(&errors, parent);
    }
    else if (guid == "{c01bd070-a483-482c-9a30-2946a4317b71}")
    {
        // Панель из текста
        TextToPanelWizard dlg(parent);

        if (dlg.exec() == QDialog::Accepted)
        {
            ResPanel *item = dlg.makePanel();
            item->setName(name);
            item->setType(LbrObject::RES_PANEL);

            pNewEditor = new StdPanelEditor(LbrObject::RES_PANEL);
            pNewEditor->setWindowIcon(RsResCore::inst()->iconFromResType(LbrObject::RES_PANEL));
            pNewEditor->setupEditor();

            ((StdPanelEditor*)pNewEditor)->setPanel(item);

            //qobject_cast<StdPanelEditor*>(pNewEditor)->setPanel(testPan);
            SetupEditorTitle(pNewEditor, LbrObject::RES_PANEL, name, item->title());
        }
    }
    else if (guid == "{80384143-d159-4264-95c8-19e8a2cf7b70}")
    {
        // Скролинг BSCROL (двойная рамка)
        pNewEditor = LoadResFromXmlTemplate(":/templates/EMPTY_BSCROL_DOUBLE.xml",
                                            name,
                                            LbrObject::RES_BS,
                                            &errors);

        if (!pNewEditor)
            ShowErrors(&errors, parent);
    }
    else if (guid == "{eaeac9f8-3230-4015-a029-ac337c3d83e9}")
    {
        // Скролинг из xml текста
        QByteArray CodeText = CodeInputDialog::getCodeText(parent, tr("Скролинг из xml текста"), tr("Введите xml:"), QString(), HighlighterXml).toUtf8();

        QBuffer buffer(&CodeText);

        if (buffer.open(QIODevice::ReadOnly))
        {
            pNewEditor = LoadResFromXmlTemplate(&buffer,
                                                name,
                                                {LbrObject::RES_BS},
                                                &errors);

            if (!pNewEditor)
                ShowErrors(&errors, parent);
        }
    }
    else if (guid == "{001b506e-4588-4810-a09a-d631fc0214d8}")
    {
        // Панель из xml текста
        QByteArray CodeText = CodeInputDialog::getCodeText(parent, tr("Панель из xml текста"), tr("Введите xml:"), QString(), HighlighterXml).toUtf8();

        QBuffer buffer(&CodeText);

        if (buffer.open(QIODevice::ReadOnly))
        {
            pNewEditor = LoadResFromXmlTemplate(&buffer,
                                                name,
                                                {LbrObject::RES_PANEL},
                                                &errors);

            if (!pNewEditor)
                ShowErrors(&errors, parent);
        }
    }

    result.wnd = pNewEditor;

    return result;
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
                        .arg(RsResCore::inst()->typeNameFromResType(Type), name, title));
}

BaseEditorWindow *BaseResourceEditor::LoadResFromXmlTemplate(QIODevice *device, const QString &name, const std::initializer_list<quint16> &type, ErrorsModel *model)
{
    static const std::vector<quint16> ScrolTypes =
        {
            LbrObject::RES_BS,
            LbrObject::RES_SCROL,
            LbrObject::RES_LS
        };

    std::vector<quint16> types;
    types.insert(types.end(), std::begin(type), std::end(type));

    BaseEditorWindow *pNewEditor = nullptr;

    auto IsIntersects = [](const std::vector<quint16>& type1, const std::vector<quint16>& type2) -> bool
    {
        std::vector<quint16> v1, v2;

        // Преобразуем оба параметра в vector
        v1.insert(v1.end(), std::begin(type1), std::end(type1));
        v2.insert(v2.end(), std::begin(type2), std::end(type2));

        std::sort(v1.begin(), v1.end());
        std::sort(v2.begin(), v2.end());

        std::vector<quint16> intersection_result;
        std::set_intersection(v1.begin(), v1.end(),
                              v2.begin(), v2.end(),
                              std::back_inserter(intersection_result));

        return !intersection_result.empty();
    };

    ResPanel *testPan = nullptr;

    try
    {
        RsResCore::inst()->loadFromXml(device, &testPan, model);
        if (testPan)
        {
            bool Create = false;
            quint16 panelType = testPan->type();

            // Проверяем, является ли тип панели одним из запрошенных типов
            if (IsIntersects({types}, {LbrObject::RES_PANEL}) &&
                IsIntersects({panelType}, types))
            {
                Create = true;
            }
            // Проверяем, является ли тип панели скролом И запрошенные типы включают скролы
            else if (IsIntersects(type, ScrolTypes) &&
                     std::find(ScrolTypes.begin(), ScrolTypes.end(), panelType) != ScrolTypes.end())
            {
                Create = true;
            }

            if (Create)
            {
                // Создаем редактор с типом панели, а не с запрошенным типом
                pNewEditor = new StdPanelEditor(panelType);
                pNewEditor->setWindowIcon(RsResCore::inst()->iconFromResType(panelType));
                pNewEditor->setupEditor();

                testPan->setName(name);
                // Не меняем тип панели - оставляем тот, что был в XML
                qobject_cast<StdPanelEditor*>(pNewEditor)->setPanel(testPan, testPan->comment());
                SetupEditorTitle(pNewEditor, panelType, name, testPan->title());
            }
            else
            {
                // Если тип не подходит, удаляем панель
                delete testPan;
                testPan = nullptr;
            }
        }
        else
        {

        }
    }
    catch(const std::exception& e)
    {
        qWarning() << "Error loading XML template:" << e.what();
        if (testPan)
            delete testPan;
    }
    catch(...)
    {
        qWarning() << "Unknown error loading XML template";
        if (testPan)
            delete testPan;
    }

    return pNewEditor;
}

BaseEditorWindow *BaseResourceEditor::LoadResFromXmlTemplate(const QString &filename,
                                                             const QString &name,
                                                             const quint16 &type,
                                                             ErrorsModel *model)
{
    BaseEditorWindow *pNewEditor = nullptr;

    try
    {
        QFile resxml(filename);
        if (!resxml.open(QIODevice::ReadOnly | QIODevice::Text))
            return nullptr;

        pNewEditor = LoadResFromXmlTemplate(&resxml, name, {type}, model);
        resxml.close();
    }
    catch(...) {}

    return pNewEditor;
}

QList<SARibbonContextCategory*> BaseResourceEditor::contextCategoryes(SARibbonBar *ribbon)
{
    SARibbonContextCategory *rescat = ribbon->addContextCategory(tr("Ресурс"), QColor(0xBFFFBF), 0);
    SARibbonContextCategory *controlcat = ribbon->addContextCategory(tr("Элемент"), QColor(0xC7FFFF), 0);
    //SARibbonContextCategory *rescat = ribbon()->addContextCategory(tr("Ресурс"), QColor(0xBFFFBF), 0);

    return { rescat, controlcat };
}
