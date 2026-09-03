#include "iconselectdialog.h"
#include "iconbrowser.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

IconSelectDialog::IconSelectDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Выбор иконки"));
    resize(640, 480);

    m_browser = new IconBrowser(this);

    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    QPushButton *okButton = buttons->button(QDialogButtonBox::Ok);
    okButton->setEnabled(m_browser->selectedIconId() != 0);

    connect(m_browser, &IconBrowser::currentIconChanged,
            okButton, [okButton](quint32 id)
    {
        okButton->setEnabled(id != 0);
    });
    connect(m_browser, &IconBrowser::iconActivated,
            this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::accepted,
            this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected,
            this, &QDialog::reject);

    QVBoxLayout *lay = new QVBoxLayout(this);
    lay->addWidget(m_browser, 1);
    lay->addWidget(buttons);
}

quint32 IconSelectDialog::selectedIconId() const
{
    return m_browser->selectedIconId();
}

void IconSelectDialog::setCurrentIconId(const quint32 &id)
{
    m_browser->setCurrentIconId(id);
}

quint32 IconSelectDialog::getIconId(QWidget *parent, const quint32 &current, bool *ok)
{
    IconSelectDialog dlg(parent);
    dlg.setCurrentIconId(current);

    const bool accepted = dlg.exec() == QDialog::Accepted;
    if (ok)
        *ok = accepted;

    return accepted ? dlg.selectedIconId() : current;
}
