#include "statusbarelement.h"
#include <QLabel>
#include <QHBoxLayout>

StatusBarElement::StatusBarElement(QWidget *parent)
    : QWidget{parent},
      m_pIcon(nullptr),
      pLayout(nullptr)
{
    m_pIcon = new QLabel(this);
    m_pText = new QLabel(this);
    pLayout = new QHBoxLayout();
    pLayout->setMargin(0);
    m_pText->setAlignment(Qt::AlignLeft);

    pLayout->addWidget(m_pIcon);
    pLayout->addWidget(m_pText, 1, Qt::AlignLeft);
    setLayout(pLayout);
}

StatusBarElement::~StatusBarElement()
{
    if (m_pIcon)
        delete m_pIcon;

    if (pLayout)
        delete pLayout;
}

void StatusBarElement::setPixmap(const QPixmap &pix)
{
    m_pIcon->setPixmap(pix);
}

void StatusBarElement::setText(const QString &str)
{
    m_pText->setText(str);
}
