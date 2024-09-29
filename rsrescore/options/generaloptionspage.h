#ifndef GENERALOPTIONSPAGE_H
#define GENERALOPTIONSPAGE_H

#include <QWidget>
#include <optionsdlg/OptionsPage.h>

namespace Ui {
class GeneralOptionsPage;
}

class PanelItem;
class OptionsScene;
class StdFolderListHandler;
class GeneralOptionsPage : public OptionsPage
{
    Q_OBJECT

public:
    GeneralOptionsPage(QWidget *parent = nullptr);
    ~GeneralOptionsPage();

    void setAutoUnloadDir(const QString &filename);

    virtual int save() Q_DECL_OVERRIDE;
    virtual void restore() Q_DECL_OVERRIDE;

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;

private:
    void UpdateGridSizesView(const int &index);
    Ui::GeneralOptionsPage *ui;

    PanelItem *panelItem;
    StdFolderListHandler *m_StdFolderListHandler;
    OptionsScene *pScene;
};

#endif // GENERALOPTIONSPAGE_H
