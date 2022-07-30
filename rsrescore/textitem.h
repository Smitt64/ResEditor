#ifndef TEXTITEM_H
#define TEXTITEM_H

#include "customrectitem.h"

class QTextDocument;
class TextItem : public CustomRectItem
{
    Q_OBJECT
    Q_CLASSINFO(CLASSINFO_UNDOREDO, "TEXT")
public:
    TextItem(QGraphicsItem* parent = nullptr);
    virtual ~TextItem();

    void setTextStruct(struct TextStruct *value);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) Q_DECL_OVERRIDE;

private:
    struct TextStruct *m_pTextStruct;
};

#endif // TEXTITEM_H
