#ifndef TEXTPAGE_H
#define TEXTPAGE_H

#include <QWizardPage>

namespace Ui {
class TextPage;
}

class TextPage : public QWizardPage
{
    Q_OBJECT

public:
    explicit TextPage(QWidget *parent = nullptr);
    ~TextPage();

    virtual bool validatePage();

private:
    Ui::TextPage *ui;
};

#endif // TEXTPAGE_H
