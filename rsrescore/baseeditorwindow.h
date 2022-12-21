#ifndef BASEEDITORWINDOW_H
#define BASEEDITORWINDOW_H

#include <QMainWindow>
#include "rsrescore_global.h"

#define CLASSINFO_TOOLBOX_FILE "TOOLBOX_FILE"

class QAction;
class QMenu;
class QToolBar;
class QUndoStack;
class QToolButton;
class UndoActionWidget;
class BaseScene;
class ToolBoxModel;
class QAbstractItemModel;
class RSRESCORE_EXPORT BaseEditorWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit BaseEditorWindow(QWidget *parent = nullptr);
    virtual ~BaseEditorWindow();
    virtual void setupEditor() = 0;

    QUndoStack *undoStack();
    ToolBoxModel *toolBox();

    QAction *undoAction();
    QAction *redoAction();

signals:
    void propertyModelChanged(QAbstractItemModel *model);

protected:
    void initUndoRedo(QToolBar *toolbar);
    void initpropertyModelSignals(BaseScene *scene);
    virtual void loadToolBox();

private:
    typedef QMap<quint16, QString> GroupsMapType;
    void loadToolBarElement(GroupsMapType &GroupsMap, const QJsonObject &obj);
    QUndoStack *m_pUndoStack;
    QAction *m_pUndoAction, *m_pRedoAction;
    QToolButton *m_pRedoActionBtn;

    ToolBoxModel *m_ToolBoxModel;

    QMenu *m_pUndoMenu;
    UndoActionWidget *m_pUndoViewMenuAction;
};

#endif // BASEEDITORWINDOW_H
