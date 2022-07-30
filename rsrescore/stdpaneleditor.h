#ifndef STDPANELEDITOR_H
#define STDPANELEDITOR_H

#include "baseeditorwindow.h"

class ResPanel;
class PanelItem;
class BaseEditorView;
class QStatusBar;
class StatusBarElement;
class QHBoxLayout;
class QLineEdit;
class StdPanelEditor : public BaseEditorWindow
{
    Q_OBJECT
public:
    StdPanelEditor(QWidget *parent = nullptr);

    virtual void setupEditor() Q_DECL_FINAL;
    void setPanel(ResPanel *panel);

private slots:
    void sceneSelectionChanged();

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
     //virtual void mouseMoveEvent(QMouseEvent *event);

private:
    void updateSizeStatus();
    void setupNameLine();
    BaseEditorView *m_pView;

    ResPanel *m_pPanel;
    PanelItem* panelItem;

    QStatusBar *m_StatusBar;
    StatusBarElement *m_SizeText, *m_CursorText;
    QWidget *m_pStatusContainer;
    QHBoxLayout *m_pStatusContainerLayout;
    QLineEdit *m_pNameLineEdit;

    QToolBar *m_pToolBar;
};

#endif // STDPANELEDITOR_H
