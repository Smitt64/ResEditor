#include "labeltexteditdlg.h"
#include "ui_labeltexteditdlg.h"
#include "widgets/characterwidget.h"

LabelTextEditDlg::LabelTextEditDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LabelTextEditDlg)
{
    ui->setupUi(this);

    m_pCharMap = new CharacterWidget(this);
    m_pCharMap->adjustSize();
    m_pCharMap->update();
    ui->scrollArea->setWidget(m_pCharMap);

    QFont font = m_pCharMap->displayFont();
    font.setPointSize(12);
    ui->lineEdit->setFont(font);
    ui->lineEdit->setMaxLength(127);

    connect(m_pCharMap, &CharacterWidget::characterSelected, [=](const QChar &character)
    {
        ui->lineEdit->insert(character);
    });
}

LabelTextEditDlg::~LabelTextEditDlg()
{
    delete ui;
}

QLineEdit *LabelTextEditDlg::editor()
{
    return ui->lineEdit;
}
