#ifndef SCROLITEM_H
#define SCROLITEM_H

#include "panelitem.h"

class ScrolItem : public PanelItem
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
    Q_INVOKABLE ScrolItem(CustomRectItem* parent = nullptr);
    virtual ~ScrolItem();

    const quint16 &rowNum() const;
    void setRowNum(const quint16 &val);

    const quint16 &rowLength() const;
    void setRowLength(const quint16 &val);

    const quint16 &rowHeight() const;
    void setRowHeight(const quint16 &val);

    const QPoint &scrolPos() const;
    void setScrolPos(const QPoint &val);

    virtual void setPanel(ResPanel *panel, const QString &comment = QString()) Q_DECL_OVERRIDE;

signals:
    void rowNumChanged();
    void rowLengthChanged();
    void rowHeightChanged();
    void scrolPosChanged();

private:
    quint16 m_RowNum, m_RowLength, m_RowHeight;
    QPoint m_ScrolPos;
};

#endif // SCROLITEM_H
