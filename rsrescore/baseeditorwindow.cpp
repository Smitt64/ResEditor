#include "baseeditorwindow.h"
#include "basescene.h"
#include "qjsonobject.h"
#include "qmetaobject.h"
#include "rsrescore.h"
#include "restemplateregistry.h"
#include "toolbox/toolboxmodel.h"
#include "SARibbon.h"
#include <QUndoStack>
#include <QToolBar>
#include <QToolButton>
#include <QMenu>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QWidgetAction>
#include <QUndoView>
#include <QMetaClassInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMimeData>
#include <QMessageBox>
#include <QCloseEvent>

class UndoActionWidget : public QWidgetAction
{
public:
    UndoActionWidget(QUndoStack *pStack, QObject *parent) :
        QWidgetAction(parent),
        m_pUndoStack(pStack)
    {

    }

protected:
    virtual QWidget *createWidget(QWidget *parent)
    {
        QUndoView *pPtr = new QUndoView(parent);
        pPtr->setStack(m_pUndoStack);
        return pPtr;
    }

private:
    QUndoStack *m_pUndoStack;
};

BaseEditorWindow::BaseEditorWindow(QWidget *parent)
    : QMainWindow{parent},
    m_pLbr(nullptr),
    m_pRibbon(nullptr)
{
    m_pUndoStack = new QUndoStack(this);
    m_ToolBoxModel = new ToolBoxModel(this);

    m_UndoIndexUnchanged = m_pUndoStack->index();
}

BaseEditorWindow::~BaseEditorWindow()
{
    clearRibbonTabs();
    /*if (m_pRibbon)
    {
        QList<SARibbonContextCategory*> lst = contextCategoryes();

        for (auto item : qAsConst(lst))
        {
            m_pRibbon->hideContextCategory(item);
            m_pRibbon->destroyContextCategory(item);
        }
    }*/
}

void BaseEditorWindow::initUndoRedo()
{
    m_pUndoMenu = new QMenu(this);
    /*m_pRedoActionBtn = new QToolButton(this);
    m_pRedoActionBtn->setMenu(m_pUndoMenu);
    m_pRedoActionBtn->setPopupMode(QToolButton::DelayedPopup);
    m_pRedoActionBtn->setIcon(QIcon(":/img/Redo.png"));*/

    m_pUndoAction = m_pUndoStack->createUndoAction(this, tr("Отменить"));
    m_pUndoAction->setToolTip(m_pUndoAction->text());
    m_pUndoAction->setIcon(QIcon(":/img/Undo.png"));
    m_pUndoAction->setShortcut(QKeySequence::Undo);
    AddShortcutToToolTip(m_pUndoAction);

    m_pRedoAction = m_pUndoStack->createRedoAction(this, tr("Повторить"));
    m_pRedoAction->setIcon(QIcon(":/img/Redo.png"));
    //m_pRedoAction->setShortcut(QKeySequence::Redo);
    m_pRedoAction->setToolTip(m_pRedoAction->text());

    /*toolbar->addAction(m_pUndoAction);
    toolbar->addWidget(m_pRedoActionBtn);

    m_pUndoViewMenuAction = new UndoActionWidget(m_pUndoStack, this);
    m_pUndoMenu->addAction(m_pRedoAction);
    m_pUndoMenu->addAction(m_pUndoViewMenuAction);

    m_pRedoActionBtn->setToolTip(tr("Повторить"));
    m_pRedoActionBtn->setShortcut(QKeySequence::Redo);

    AddShortcutToToolTip(m_pRedoActionBtn);*/

    connect(m_pUndoStack, &QUndoStack::indexChanged, [=](const int index) -> void
    {
        emit modifyChanged(index != m_UndoIndexUnchanged);
        //emit modifyChanged(!m_pUndoStack->isClean());
    });
    //connect(m_pRedoActionBtn, &QToolButton::clicked, m_pRedoAction, &QAction::trigger);
}

bool BaseEditorWindow::isChanged() const
{
    return m_pUndoStack->index() != m_UndoIndexUnchanged;
}

bool BaseEditorWindow::save(ResBuffer *res, QString *error)
{
    Q_UNUSED(res)
    m_UndoIndexUnchanged = m_pUndoStack->index();
    emit modifyChanged(false);
    return true;
}

QUndoStack *BaseEditorWindow::undoStack()
{
    return m_pUndoStack;
}

ToolBoxModel *BaseEditorWindow::toolBox()
{
    return m_ToolBoxModel;
}

QAbstractItemModel *BaseEditorWindow::propertyModel()
{
    return nullptr;
}

QAbstractItemModel *BaseEditorWindow::structModel()
{
    return nullptr;
}

QAction *BaseEditorWindow::undoAction()
{
    return m_pUndoAction;
}

QAction *BaseEditorWindow::redoAction()
{
    return m_pRedoAction;
}

QToolButton *BaseEditorWindow::redoActionBtn()
{
    return m_pRedoActionBtn;
}

QString BaseEditorWindow::name() const
{
    return QString();
}

QString BaseEditorWindow::title() const
{
    return QString("Unnamed");
}

qint16 BaseEditorWindow::type() const
{
    return -1;
}

void BaseEditorWindow::initpropertyModelSignals(BaseScene *scene)
{
    connect(scene, &BaseScene::propertyModelChanged, this, &BaseEditorWindow::propertyModelChanged);
    connect(scene, &BaseScene::propertyModelChanged, this, &BaseEditorWindow::OnPropertyModelChanged);
}

void BaseEditorWindow::OnPropertyModelChanged(QAbstractItemModel *model)
{
    ribbon()->setUpdatesEnabled(false);
    clearRibbonTabs();
    updateRibbonTabs();
    ribbon()->setUpdatesEnabled(true);
}

void BaseEditorWindow::loadToolBarElement(GroupsMapType &GroupsMap, const QJsonObject &obj, const QDir &baseDir)
{
    QString alias = obj["alias"].toString();
    QString iconSrc = obj["icon"].toString();
    QString mimetype = obj["mimetype"].toString();
    QJsonObject mimedata = obj["mimedata"].toObject();
    QStringList category;

    QJsonArray groupsObj = obj["groups"].toArray();
    for (const QJsonValue &value : qAsConst(groupsObj))
    {
        int id = value.toInt();
        QString categ = GroupsMap[id];
        category.append(categ);
    }

    // Иконка: "theme:Имя" — из иконочной темы; относительный путь —
    // резолвится от каталога json-файла (пользовательские элементы
    // кладут свои svg рядом с json)
    QIcon icon;
    if (iconSrc.startsWith(QLatin1String("theme:")))
        icon = QIcon::fromTheme(iconSrc.mid(6));
    else if (!iconSrc.isEmpty() && !iconSrc.startsWith(QLatin1String(":/")) &&
             !QDir::isAbsolutePath(iconSrc))
        icon = QIcon(QDir::fromNativeSeparators(baseDir.absoluteFilePath(iconSrc)));
    else
        icon = QIcon(iconSrc);

    QJsonDocument doc(mimedata);
    QMimeData mdata;
    mdata.setData(mimetype, doc.toJson());
    m_ToolBoxModel->addItem(category, alias, &mdata, icon);
}

void BaseEditorWindow::loadToolBox()
{
    int infoIndex = metaObject()->indexOfClassInfo(CLASSINFO_TOOLBOX_FILE);

    if (infoIndex >= 0)
    {
        QString fname = metaObject()->classInfo(infoIndex).value();
        if (!fname.isEmpty())
            loadToolBoxFile(fname);
    }

    // Пользовательские элементы toolbox: <user|working|program>/toolbox/<id>/*.json
    // Грузятся после встроенного qrc-файла — могут ссылаться на его
    // группы по title; семантика аддитивная (переопределения нет)
    const QString id = toolBoxId();
    if (!id.isEmpty())
    {
        const QStringList dirs =
        {
            ResTemplateRegistry::userToolBoxDir(id),
            ResTemplateRegistry::workingToolBoxDir(id),
            ResTemplateRegistry::programToolBoxDir(id)
        };

        for (const QString &dirPath : dirs)
        {
            QDir dir(dirPath);
            if (!dir.exists())
                continue;

            const QFileInfoList files = dir.entryInfoList(QStringList() << QStringLiteral("*.json"),
                                                          QDir::Files | QDir::Readable, QDir::Name);
            for (const QFileInfo &fi : files)
                loadToolBoxFile(fi.absoluteFilePath());
        }
    }
}

QString BaseEditorWindow::toolBoxId() const
{
    return QString();
}

QPixmap BaseEditorWindow::toolBoxDragPixmap(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QPixmap();
}

void BaseEditorWindow::loadToolBoxFile(const QString &fname, const GroupsMapType &outergroups)
{
    QFile file(fname);
    if (!file.open(QIODevice::ReadOnly))
        return;

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError)
    {
        qWarning("ToolBox: %s: %s", qPrintable(fname), qPrintable(parseError.errorString()));
        return;
    }
    QJsonObject root = doc.object();

    GroupsMapType GroupsMap = outergroups;
    QJsonArray groups = root["groups"].toArray();
    for (const QJsonValue &group : qAsConst(groups))
    {
        QJsonObject groupobj = group.toObject();
        int id = groupobj["id"].toInt();
        QString name = groupobj["title"].toString();
        GroupsMap[id] = name;

        // Группа с таким названием уже есть (встроенная или из другого
        // пользовательского файла) — переиспользуем, дубликат не создаём
        if (!m_ToolBoxModel->category(name))
            m_ToolBoxModel->addCategory(name);
    }

    const QDir baseDir = QFileInfo(fname).absoluteDir();

    QJsonArray templates = root["templates"].toArray();
    for (const QJsonValue &templ : qAsConst(templates))
        loadToolBarElement(GroupsMap, templ.toObject(), baseDir);

    QJsonArray includes = root["includes"].toArray();
    for (const QJsonValue &templ : qAsConst(includes))
        loadToolBoxFile(templ.toString(), GroupsMap);
}

void BaseEditorWindow::setLbrObject(LbrObjectInterface *obj)
{
    m_pLbr = obj;
}

LbrObjectInterface *BaseEditorWindow::lbr()
{
    return m_pLbr;
}

void BaseEditorWindow::setRibbonBar(SARibbonBar *ribbon)
{
    m_pRibbon = ribbon;
    initRibbonPanels();
}

SARibbonBar *BaseEditorWindow::ribbon()
{
    return m_pRibbon;
}

SARibbonContextCategory *BaseEditorWindow::findCategoryByName(const QString &name)
{
    QList<SARibbonContextCategory*> lst = m_pRibbon->contextCategoryList();

    for (auto item : std::as_const(lst))
    {
        if (item->contextTitle() == name)
            return item;
    }

    return nullptr;
}

void BaseEditorWindow::initRibbonPanels()
{

}

void BaseEditorWindow::updateRibbonTabs()
{

}

void BaseEditorWindow::clearRibbonTabs()
{

}

QList<QWidget*> BaseEditorWindow::statusBarSections()
{
    return QList<QWidget*>();
}

QAction* BaseEditorWindow::createAction(const QString& text, const QString& iconname, const QKeySequence &key)
{
    QAction* act = new QAction(this);
    act->setText(text);
    act->setIcon(QIcon::fromTheme(iconname));
    act->setObjectName("obj_" + text);

    if (!key.isEmpty())
        act->setShortcut(key);

    return act;
}
