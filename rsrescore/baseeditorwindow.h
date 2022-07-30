#ifndef BASEEDITORWINDOW_H
#define BASEEDITORWINDOW_H

#include <QMainWindow>

class QAction;
class QMenu;
class QToolBar;
class QUndoStack;
class QToolButton;
class UndoActionWidget;
class BaseEditorWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit BaseEditorWindow(QWidget *parent = nullptr);
    virtual ~BaseEditorWindow();
    virtual void setupEditor() = 0;

    QUndoStack *undoStack();

signals:

protected:
    void initUndoRedo(QToolBar *toolbar);

private:
    QUndoStack *m_pUndoStack;
    QAction *m_pUndoAction, *m_pRedoAction;
    QToolButton *m_pRedoActionBtn;

    QMenu *m_pUndoMenu;
    UndoActionWidget *m_pUndoViewMenuAction;
};

#endif // BASEEDITORWINDOW_H
