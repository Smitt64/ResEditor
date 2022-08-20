#ifndef UNDOPROPERTYCHANGE_H
#define UNDOPROPERTYCHANGE_H

#include <QUndoCommand>
#include <QUuid>
#include <QVariant>

class CustomRectItem;
class BaseScene;
class UndoPropertyChange : public QUndoCommand
{
public:
    UndoPropertyChange(BaseScene *scene, const QUuid &uuid, QUndoCommand *parent = nullptr);
    virtual ~UndoPropertyChange();

    void setValues(const QVariant &old, const QVariant &new_);
    void setPropertyName(const QString &name);

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    BaseScene *m_pScene;

    QUuid m_ItemId;
    QVariant m_OldValue, m_NewValue;
    QString m_PropertyName;
};

#endif // UNDOPROPERTYCHANGE_H
