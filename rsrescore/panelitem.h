#ifndef PANELITEM_H
#define PANELITEM_H

#include <containeritem.h>

class ResPanel;
class QTextDocument;
class PanelItem : public ContainerItem
{
    Q_OBJECT
    Q_CLASSINFO(CLASSINFO_UNDOREDO, "PANEL")
public:
    PanelItem(CustomRectItem* parent = nullptr);
    virtual ~PanelItem();

    void setPanel(ResPanel *panel);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) Q_DECL_OVERRIDE;

private:
    ResPanel *m_Panel;
    QTextDocument *m_pDocument;
};

#endif // PANELITEM_H
