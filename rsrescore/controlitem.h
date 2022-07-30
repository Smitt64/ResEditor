#ifndef CONTROLITEM_H
#define CONTROLITEM_H

#include "customrectitem.h"
#include <QObject>

class ControlItem : public CustomRectItem
{
    Q_OBJECT
    Q_CLASSINFO(CLASSINFO_UNDOREDO, "CONTROL")
public:
    ControlItem(QGraphicsItem* parent = nullptr);
    virtual ~ControlItem();

    void setFieldStruct(struct FieldStruct *value);

    static void paintControl(const quint8 &FType, QPainter *painter, const QRectF &rect);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) Q_DECL_OVERRIDE;

private:
    struct FieldStruct *m_pFieldStruct;
};

#endif // CONTROLITEM_H
