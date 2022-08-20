#include "baseeditorwindow.h"
#include "basescene.h"
#include <QUndoStack>
#include <QToolBar>
#include <QToolButton>
#include <QMenu>
#include <QWidgetAction>
#include <QUndoView>

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

    m_pRedoAction = m_pUndoStack->createRedoAction(this, tr("Повторить"));
    m_pRedoAction->setIcon(QIcon(":/img/Redo.png"));

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

void BaseEditorWindow::initpropertyModelSignals(BaseScene *scene)
{
    connect(scene, &BaseScene::propertyModelChanged, this, &BaseEditorWindow::propertyModelChanged);
}
