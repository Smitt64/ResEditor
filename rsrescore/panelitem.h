#ifndef PANELITEM_H
#define PANELITEM_H

#include <containeritem.h>
#include "styles/resstyle.h"

#define MIMETYPE_TOOLBOX "application/toolboxitem"

class ResPanel;
class QTextDocument;
class PanelPropertysDlg;
class PanelItem : public ContainerItem
{
    Q_OBJECT
    Q_FLAGS(PanelExcludeFlags)
    Q_PROPERTY(ResStyle::PanelStyle panelStyle READ panelStyle WRITE setPanelStyle NOTIFY panelStyleChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(QString status2 READ status2 WRITE setStatus2 NOTIFY status2Changed)
    Q_PROPERTY(bool isCentered READ isCentered WRITE setIsCentered NOTIFY isCenteredChanged)
    Q_PROPERTY(bool isRightText READ isRightText WRITE setIsRightText NOTIFY isRightTextChanged)
    Q_PROPERTY(PanelExcludeFlags panelExclude READ panelExclude WRITE setPanelExclude NOTIFY panelExcludeChanged)
    Q_PROPERTY(QString comment READ comment WRITE setComment NOTIFY commentChanged)
    Q_PROPERTY(quint16 helpPage READ helpPage WRITE setHelpPage NOTIFY helpPageChanged)

    Q_CLASSINFO(CLASSINFO_UNDOREDO, "PANEL")
    Q_CLASSINFO(CLASSINFO_PROPERTYLIST, ":/json/Panel.json")
    Q_CLASSINFO(CLASSINFO_PROPERTYGROUP, "Panel")
public:
    enum PanelExcludeFlag
    {
        ExcludeNavigation = 0x0001,
        ExcludeAutoNum = 0x0002,
        ExcludeShadow = 0x0004
    };
    Q_ENUM(PanelExcludeFlag)
    Q_DECLARE_FLAGS(PanelExcludeFlags, PanelExcludeFlag)

    Q_INVOKABLE PanelItem(CustomRectItem* parent = nullptr);
    virtual ~PanelItem();

    virtual void setPanel(ResPanel *panel, const QString &comment = QString());

    const ResStyle::PanelStyle &panelStyle() const;
    void setPanelStyle(const ResStyle::PanelStyle &style);

    QString title() const;
    void setTitle(const QString &text);

    QString status() const;
    void setStatus(const QString &text);

    QString status2() const;
    void setStatus2(const QString &text);

    QString comment() const;
    void setComment(const QString &text);

    const bool &isCentered() const;
    void setIsCentered(const bool &val);

    const bool &isRightText() const;
    void setIsRightText(const bool &val);

    PanelExcludeFlags panelExclude() const;
    void setPanelExclude(const PanelItem::PanelExcludeFlags &val);

    const quint16 &helpPage() const;
    void setHelpPage(const quint16 &val);

    virtual QVariant userAction(const qint32 &action, const QVariant &param = QVariant()) Q_DECL_OVERRIDE;

    void updateChildControlsOrder();

signals:
    void panelStyleChanged();
    void titleChanged();
    void statusChanged();
    void status2Changed();
    void isCenteredChanged();
    void isRightTextChanged();
    void commentChanged();
    void panelExcludeChanged();
    void helpPageChanged();

    void childAdded(CustomRectItem *item);
    void childRemoved(CustomRectItem *item);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) Q_DECL_OVERRIDE;
    virtual bool event(QEvent *e) Q_DECL_OVERRIDE;
    //virtual void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;
    virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;
    virtual void dropEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;
    virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;

    virtual void FillItemPanel(PanelPropertysDlg &dlg);

private:
    virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) Q_DECL_OVERRIDE;
    void setChildsVisible(const bool &value);

    ResPanel *m_Panel;
    ResStyle::PanelStyle m_PanelStyle;
    QString m_Title, m_Status, m_Status2, m_Comment;
    PanelExcludeFlags m_PanelExclude;
    quint16 m_HelpPage;

    bool m_isCentered, m_isRightText;

    QRectF m_DragHighlightedRect;
    QPixmap *m_DragPixmap, *m_DragControl;
};

Q_DECLARE_OPAQUE_POINTER(PanelItem)

Q_DECLARE_OPERATORS_FOR_FLAGS(PanelItem::PanelExcludeFlags)
Q_DECLARE_METATYPE(PanelItem::PanelExcludeFlags)

#endif // PANELITEM_H
