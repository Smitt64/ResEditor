#include "bankdistribselect.h"
#include "ui_bankdistribselect.h"
#include <QSettings>
#include <QTreeWidgetItem>
#include <QDir>
#include <QFileDialog>
#include <QAbstractButton>

#include "resapplication.h"

BankDistribSelect::BankDistribSelect(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BankDistribSelect)
{
    ui->setupUi(this);

    QTreeWidgetItem *userItem = ui->treeWidget->topLevelItem(0);
    QTreeWidgetItem *autoItem = ui->treeWidget->topLevelItem(1);
    QString basePath = "HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
    QSettings keys(basePath, QSettings::NativeFormat);

    QStringList keysGroup = keys.childGroups();
    for (const QString &key : qAsConst(keysGroup))
    {
        keys.beginGroup(key);

        QString DisplayName = keys.value("DisplayName").toString();
        QString Publisher = keys.value("Publisher").toString();
        QString DisplayVersion = keys.value("DisplayVersion").toString();

        DisplayVersion = DisplayVersion.mid(DisplayVersion.indexOf("31"));
        if (Publisher.contains("R-Style Softlab") && (DisplayName.contains("RS-Bank") || DisplayName.contains("RS-FinMarkets")))
        {
            QDir d(keys.value("InstallLocation").toString());

            if (d.cd("obj"))
            {
                QTreeWidgetItem *item = new QTreeWidgetItem();
                item->setText(0, DisplayName);
                item->setData(0, Qt::UserRole, d.absolutePath());
                item->setIcon(0, QIcon(":/img/rsbank.png"));

                autoItem->addChild(item);
            }
        }
        keys.endGroup();
    }

    int size = ResApp->settings()->beginReadArray("BankDistribSelect");
    for (int i = 0; i < size; ++i)
    {
        ResApp->settings()->setArrayIndex(i);

        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, ResApp->settings()->value("BankDistrib").toString());
        item->setData(0, Qt::UserRole, item->text(0));

        userItem->addChild(item);
    }
    ResApp->settings()->endArray();

    ui->treeWidget->expandAll();

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

BankDistribSelect::~BankDistribSelect()
{
    delete ui;
}

void BankDistribSelect::on_pushButton_clicked()
{
    QTreeWidgetItem *userItem = ui->treeWidget->topLevelItem(0);
    QString path = QFileDialog::getExistingDirectory(this);

    if (!path.isEmpty())
    {
        QDir dir(path);

        if (QFile::exists(dir.absoluteFilePath("startbnk.exe")))
        {
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, dir.path());
            item->setData(0, Qt::UserRole, item->text(0));

            userItem->addChild(item);
        }
    }

    ResApp->settings()->beginWriteArray("BankDistribSelect");
    for (int i = 0; i < userItem->childCount(); i++)
    {
        ResApp->settings()->setArrayIndex(i);

        QTreeWidgetItem *item = userItem->child(i);
        ResApp->settings()->setValue("BankDistrib", item->data(0, Qt::UserRole).toString());
    }
    ResApp->settings()->endArray();

    ResApp->settings()->sync();
}


void BankDistribSelect::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

QString BankDistribSelect::path() const
{
    return ui->treeWidget->currentItem()->data(0, Qt::UserRole).toString();
}

void BankDistribSelect::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if (!item)
        return;

    accept();
}

