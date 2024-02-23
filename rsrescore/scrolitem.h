#ifndef SCROLITEM_H
#define SCROLITEM_H

#include "panelitem.h"
#include "lbrobject.h"

class ScrolItem;
class ScrolAreaRectItem : public CustomRectItem
{
    Q_OBJECT

    Q_PROPERTY(quint16 rowNum READ rowNum WRITE setRowNum NOTIFY rowNumChanged)
    Q_PROPERTY(quint16 rowLength READ rowLength WRITE setRowLength NOTIFY rowLengthChanged)
    Q_PROPERTY(quint16 rowHeight READ rowHeight WRITE setRowHeight NOTIFY rowHeightChanged)
    Q_PROPERTY(QPoint scrolPos READ scrolPos WRITE setScrolPos NOTIFY scrolPosChanged)

    Q_CLASSINFO(CLASSINFO_UNDOREDO, "SCROL")
    Q_CLASSINFO(CLASSINFO_PROPERTYLIST, ":/json/Scrol.json")
    Q_CLASSINFO(CLASSINFO_PROPERTYGROUP, "Scrol")
public:
    ScrolAreaRectItem(CustomRectItem* parent);
    virtual ~ScrolAreaRectItem();

    const quint16 &rowNum() const;
    void setRowNum(const quint16 &val);

    const quint16 &rowLength() const;
    void setRowLength(const quint16 &val);

    const quint16 &rowHeight() const;
    void setRowHeight(const quint16 &val);

    const QPoint &scrolPos() const;
    void setScrolPos(const QPoint &val);

signals:
    void rowNumChanged();
    void rowLengthChanged();
    void rowHeightChanged();
    void scrolPosChanged();

protected:
    virtual bool isIntersects(const QRectF &thisBound, QGraphicsItem *item, const QRectF &itemBound) const Q_DECL_OVERRIDE;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) Q_DECL_OVERRIDE;

private:
    ScrolItem *m_Scrol;
};

class ScrolAreaRectItem;
class ScrolItem : public PanelItem
{
    Q_OBJECT
    Q_FLAGS(ScrolFlags)
    Q_PROPERTY(ScrolType scrolType READ scrolType WRITE setScrolType NOTIFY scrolTypeChanged)
    Q_PROPERTY(quint16 rowNum READ rowNum WRITE setRowNum NOTIFY rowNumChanged)
    Q_PROPERTY(quint16 rowLength READ rowLength WRITE setRowLength NOTIFY rowLengthChanged)
    Q_PROPERTY(quint16 rowHeight READ rowHeight WRITE setRowHeight NOTIFY rowHeightChanged)
    Q_PROPERTY(QPoint scrolPos READ scrolPos WRITE setScrolPos NOTIFY scrolPosChanged)
    Q_PROPERTY(ScrolFlags scrolFlags READ scrolFlags WRITE setScrolFlags NOTIFY scrolFlagsChanged) 

    Q_CLASSINFO(CLASSINFO_UNDOREDO, "SCROL")
    Q_CLASSINFO(CLASSINFO_PROPERTYLIST, ":/json/Scrol.json")
    Q_CLASSINFO(CLASSINFO_PROPERTYGROUP, "Scrol")
public:
    enum ScrolFlag
    {
        ScrolAutoFill    = 0x00010000, // расширять панель и обл. скролинга до экрана
        ScrolAutoFields  = 0x00020000, // автоматически устанавливать кординать полей в скролинге
        ScrolAutoHeader  = 0x00040000, // генерировать заголовки колонок в скролинге
        ScrolReversOrder = 0x00080000, // обратный порядок записей
        ScrolDenySort    = 0x01000000, // запретить сортировку скролинга
    };

    enum ScrolType
    {
        TypeSCROL = LbrObject::RES_SCROL,
        TypeBSCROL = LbrObject::RES_BS,
        TypeLSCROL = LbrObject::RES_LS,
    };

    Q_ENUM(ScrolFlag)
    Q_ENUM(ScrolType)
    Q_DECLARE_FLAGS(ScrolFlags, ScrolFlag)

    Q_INVOKABLE ScrolItem(CustomRectItem* parent = nullptr);
    virtual ~ScrolItem();

    const ScrolType &scrolType() const;
    void setScrolType(const ScrolType &val);

    const quint16 &rowNum() const;
    void setRowNum(const quint16 &val);

    const quint16 &rowLength() const;
    void setRowLength(const quint16 &val);

    const quint16 &rowHeight() const;
    void setRowHeight(const quint16 &val);

    const QPoint &scrolPos() const;
    void setScrolPos(const QPoint &val);

    const ScrolFlags &scrolFlags() const;
    void setScrolFlags(const ScrolFlags &val);

    virtual void setPanel(ResPanel *panel, const QString &comment = QString()) Q_DECL_OVERRIDE;
    virtual QVariant userAction(const qint32 &action, const QVariant &param = QVariant()) Q_DECL_OVERRIDE;

public slots:
    void showScrolArea(bool visible);

protected:
    virtual void FillItemPanel(PanelPropertysDlg &dlg) Q_DECL_OVERRIDE;
    virtual void createItemResizeUndoObj(BaseScene* customScene,
                                         const QSizeF &Actual,
                                         const QSizeF &New,
                                         QUndoCommand **cmd) Q_DECL_OVERRIDE;

    virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;
    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;

signals:
    void rowNumChanged();
    void rowLengthChanged();
    void rowHeightChanged();
    void scrolPosChanged();
    void scrolFlagsChanged();
    void scrolTypeChanged();

private:
    ScrolType m_Type;
    quint16 m_RowNum, m_RowLength, m_RowHeight;
    QPoint m_ScrolPos;
    bool m_IsScrolAreaVisible;

    ScrolFlags m_ScrolFlags;
    ScrolAreaRectItem *m_ScrolArea;
};

Q_DECLARE_OPAQUE_POINTER(ScrolItem)

Q_DECLARE_OPERATORS_FOR_FLAGS(ScrolItem::ScrolFlags)
Q_DECLARE_METATYPE(ScrolItem::ScrolFlags)

#endif // SCROLITEM_H
