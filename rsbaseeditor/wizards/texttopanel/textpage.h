#ifndef TEXTPAGE_H
#define TEXTPAGE_H

#include <QWizardPage>

namespace Ui {
class TextPage;
}

class ResPanel;
class QTextCursor;
class TextPage : public QWizardPage
{
    Q_OBJECT
    friend class TextToPanelWizard;
public:
    explicit TextPage(QWidget *parent = nullptr);
    ~TextPage();

    virtual bool validatePage();

private:
    ResPanel *m_pPanel;
    Ui::TextPage *ui;
};

#endif // TEXTPAGE_H
