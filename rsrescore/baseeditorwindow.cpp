#include "baseeditorwindow.h"
#include "basescene.h"
#include "qjsonobject.h"
#include "qmetaobject.h"
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
    : QMainWindow{parent}
{
    m_pUndoStack = new QUndoStack(this);
    m_ToolBoxModel = new ToolBoxModel(this);
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
    m_pUndoAction->setIcon(QIcon(":/img/Undo.png"));
    m_pUndoAction->setShortcut(QKeySequence::Undo);

    m_pRedoAction = m_pUndoStack->createRedoAction(this, tr("Повторить"));
    m_pRedoAction->setIcon(QIcon(":/img/Redo.png"));
    m_pRedoAction->setShortcut(QKeySequence::Redo);

    toolbar->addAction(m_pUndoAction);
    toolbar->addWidget(m_pRedoActionBtn);

    m_pUndoViewMenuAction = new UndoActionWidget(m_pUndoStack, this);
    m_pUndoMenu->addAction(m_pRedoAction);
    m_pUndoMenu->addAction(m_pUndoViewMenuAction);

    connect(m_pRedoActionBtn, &QToolButton::clicked, m_pRedoAction, &QAction::trigger);
}

QUndoStack *BaseEditorWindow::undoStack()
{
    return m_pUndoStack;
}

ToolBoxModel *BaseEditorWindow::toolBox()
{
    return m_ToolBoxModel;
}

QAction *BaseEditorWindow::undoAction()
{
    return m_pUndoAction;
}

QAction *BaseEditorWindow::redoAction()
{
    return m_pRedoAction;
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
