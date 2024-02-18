#include "baseeditorwindow.h"
#include "basescene.h"
#include "qjsonobject.h"
#include "qmetaobject.h"
#include "rsrescore.h"
#include "toolbox/toolboxmodel.h"
#include <QUndoStack>
#include <QToolBar>
#include <QToolButton>
#include <QMenu>
#include <QFile>
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
    m_pLbr(nullptr)
{
    m_pUndoStack = new QUndoStack(this);
    m_ToolBoxModel = new ToolBoxModel(this);

    m_UndoIndexUnchanged = m_pUndoStack->index();
}

BaseEditorWindow::~BaseEditorWindow()
{

}

void BaseEditorWindow::initUndoRedo(QToolBar *toolbar)
{
    m_pUndoMenu = new QMenu(this);
    m_pRedoActionBtn = new QToolButton(this);
    m_pRedoActionBtn->setMenu(m_pUndoMenu);
    m_pRedoActionBtn->setPopupMode(QToolButton::DelayedPopup);
    m_pRedoActionBtn->setIcon(QIcon(":/img/Redo.png"));

    m_pUndoAction = m_pUndoStack->createUndoAction(this, tr("Отменить"));
    m_pUndoAction->setToolTip(m_pUndoAction->text());
    m_pUndoAction->setIcon(QIcon(":/img/Undo.png"));
    m_pUndoAction->setShortcut(QKeySequence::Undo);
    AddShortcutToToolTip(m_pUndoAction);

    m_pRedoAction = m_pUndoStack->createRedoAction(this, tr("Повторить"));
    m_pRedoAction->setIcon(QIcon(":/img/Redo.png"));
    //m_pRedoAction->setShortcut(QKeySequence::Redo);
    m_pRedoAction->setToolTip(m_pRedoAction->text());

    toolbar->addAction(m_pUndoAction);
    toolbar->addWidget(m_pRedoActionBtn);

    m_pUndoViewMenuAction = new UndoActionWidget(m_pUndoStack, this);
    m_pUndoMenu->addAction(m_pRedoAction);
    m_pUndoMenu->addAction(m_pUndoViewMenuAction);

    m_pRedoActionBtn->setToolTip(tr("Повторить"));
    m_pRedoActionBtn->setShortcut(QKeySequence::Redo);

    AddShortcutToToolTip(m_pRedoActionBtn);

    connect(m_pUndoStack, &QUndoStack::indexChanged, [=](const int index) -> void
    {
        emit modifyChanged(index != m_UndoIndexUnchanged);
        //emit modifyChanged(!m_pUndoStack->isClean());
    });
    connect(m_pRedoActionBtn, &QToolButton::clicked, m_pRedoAction, &QAction::trigger);
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

QAction *BaseEditorWindow::undoAction()
{
    return m_pUndoAction;
}

QAction *BaseEditorWindow::redoAction()
{
    return m_pRedoAction;
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
}

void BaseEditorWindow::loadToolBarElement(GroupsMapType &GroupsMap, const QJsonObject &obj)
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

    QJsonDocument doc(mimedata);
    QMimeData mdata;
    mdata.setData(mimetype, doc.toJson());
    m_ToolBoxModel->addItem(category, alias, &mdata, QIcon(iconSrc));
}

void BaseEditorWindow::loadToolBox()
{
    int infoIndex = metaObject()->indexOfClassInfo(CLASSINFO_TOOLBOX_FILE);

    if (infoIndex < 0)
        return;

    QString fname = metaObject()->classInfo(infoIndex).value();
    if (fname.isEmpty())
        return;

    QFile file(fname);
    if (!file.open(QIODevice::ReadOnly))
        return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject root = doc.object();

    GroupsMapType GroupsMap;
    QJsonArray groups = root["groups"].toArray();
    for (const QJsonValue &group : qAsConst(groups))
    {
        QJsonObject groupobj = group.toObject();
        int id = groupobj["id"].toInt();
        QString name = groupobj["title"].toString();
        GroupsMap[id] = name;
        m_ToolBoxModel->addCategory(name);
    }

    QJsonArray templates = root["templates"].toArray();
    for (const QJsonValue &templ : qAsConst(templates))
        loadToolBarElement(GroupsMap, templ.toObject());
}

void BaseEditorWindow::setLbrObject(LbrObjectInterface *obj)
{
    m_pLbr = obj;
}

LbrObjectInterface *BaseEditorWindow::lbr()
{
    return m_pLbr;
}
