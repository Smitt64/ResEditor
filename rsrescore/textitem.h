#ifndef TEXTITEM_H
#define TEXTITEM_H

#include "customrectitem.h"

#define TEXTITEM_HORIZONTAL "─"
class TextItem : public CustomRectItem
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(TextItemType textType READ textType NOTIFY textTypeChanged)

    Q_CLASSINFO(CLASSINFO_UNDOREDO, "TEXT")
    Q_CLASSINFO(CLASSINFO_PROPERTYLIST, ":/json/LabelItem.json")
    Q_CLASSINFO(CLASSINFO_PROPERTYGROUP, "Label")
public:
    enum TextItemType
    {
        TypeLabel = 0,
        TypeCheck,
        TypeRadio,
        TypeHorizontal,
    };
    Q_ENUM(TextItemType)
    Q_INVOKABLE TextItem(QGraphicsItem* parent = nullptr);
    virtual ~TextItem();

    void setTextStruct(struct TextStruct *value);

    const TextItemType &textType() const;

    QString text() const;
    void setText(const QString &txt);

    //virtual void serialize(QByteArray &data) Q_DECL_OVERRIDE;

signals:
    void textChanged();
    void textTypeChanged();

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) Q_DECL_OVERRIDE;
    virtual bool canResize(const QRectF &newRect, const ResizeCorners &corner) const Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;

private:
    void detectType();
    QString checkRadioStr(const TextItemType &type) const;
    //struct TextStruct *m_pTextStruct;
    QString m_Value;

    TextItemType m_Type;
    int m_CheckRadioPos;
};

Q_DECLARE_OPAQUE_POINTER(TextItem)

#endif // TEXTITEM_H
