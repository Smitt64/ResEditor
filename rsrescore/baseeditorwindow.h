#ifndef BASEEDITORWINDOW_H
#define BASEEDITORWINDOW_H

#include <QMainWindow>
#include "rsrescore_global.h"
#include <QMap>

#define CLASSINFO_TOOLBOX_FILE "TOOLBOX_FILE"

class QAction;
class QMenu;
class QToolBar;
class QUndoStack;
class QToolButton;
class UndoActionWidget;
class BaseScene;
class ToolBoxModel;
class ResBuffer;
class QAbstractItemModel;
class LbrObjectInterface;
class RSRESCORE_EXPORT BaseEditorWindow : public QMainWindow
{
    Q_OBJECT

    typedef QMap<quint16, QString> GroupsMapType;
public:
    explicit BaseEditorWindow(QWidget *parent = nullptr);
    virtual ~BaseEditorWindow();
    virtual void setupEditor() = 0;
    virtual bool save(ResBuffer *res, QString *error);

    QUndoStack *undoStack();
    ToolBoxModel *toolBox();
    virtual QAbstractItemModel *propertyModel();
    virtual QAbstractItemModel *structModel();

    QAction *undoAction();
    QAction *redoAction();

    virtual QString name() const;
    virtual QString title() const;
    virtual qint16 type() const;

    bool isChanged() const;

    void setLbrObject(LbrObjectInterface *obj);
    LbrObjectInterface *lbr();

signals:
    void propertyModelChanged(QAbstractItemModel *model);
    void readySave(bool closeAfterSave = false);
    void modifyChanged(bool changed);
    void titleChanged(QString);

protected:
    void initUndoRedo(QToolBar *toolbar);
    void initpropertyModelSignals(BaseScene *scene);
    virtual void loadToolBox();
    void loadToolBoxFile(const QString &fname, const GroupsMapType &outergroups = GroupsMapType());

private:
    void loadToolBarElement(GroupsMapType &GroupsMap, const QJsonObject &obj);
    QUndoStack *m_pUndoStack;
    QAction *m_pUndoAction, *m_pRedoAction;
    QToolButton *m_pRedoActionBtn;

    ToolBoxModel *m_ToolBoxModel;

    QMenu *m_pUndoMenu;
    int m_UndoIndexUnchanged;
    UndoActionWidget *m_pUndoViewMenuAction;
    LbrObjectInterface *m_pLbr;
};

#endif // BASEEDITORWINDOW_H
