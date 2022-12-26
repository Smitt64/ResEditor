#include "textstyledlg.h"
#include "ui_textstyledlg.h"
#include "easywinini.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QButtonGroup>
#include <QFile>
#include <QMdiSubWindow>
#include <QUiLoader>

TextStyleColorModel::TextStyleColorModel(QObject *parent) :
    EnumListModel(parent)
{
    QFile f(":/json/TextColorEnum.json");

    if (f.open(QIODevice::ReadOnly))
    {
        QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
        QJsonObject root = doc.object();

        loadFromJsonArray(root["enum"].toArray());
    }
}

TextStyleColorModel::~TextStyleColorModel()
{

}

QVariant TextStyleColorModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DecorationRole)
    {
        QString key = index.data(EnumListModel::RoleKey).toString();
        int lastIndex = key.lastIndexOf("_");
        key = key.mid(lastIndex + 1);

        return easyWin->textStyleColor(key);
    }

    return EnumListModel::data(index, role);
}

// ==============================================================

TextStyleBackModel::TextStyleBackModel(QObject *parent) :
    EnumListModel(parent)
{
    QFile f(":/json/TextBackEnum.json");

    if (f.open(QIODevice::ReadOnly))
    {
        QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
        QJsonObject root = doc.object();

        loadFromJsonArray(root["enum"].toArray());
    }
}

TextStyleBackModel::~TextStyleBackModel()
{

}

QVariant TextStyleBackModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DecorationRole)
    {
        QString key = index.data(EnumListModel::RoleKey).toString();

        if (key != "dBACK_STYLE_WHITE")
        {
            int lastIndex = key.lastIndexOf("_");
            key = key.mid(lastIndex + 1);

            return easyWin->textStyleColor(key);
        }
        else
            return QColor(Qt::transparent);
    }

    return EnumListModel::data(index, role);
}

// ==============================================================

TextStyleDlg::TextStyleDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TextStyleDlg)
{
    ui->setupUi(this);

    m_pJustify = new QButtonGroup(this);
    m_pJustify->addButton(ui->toolAlignCenter);
    m_pJustify->addButton(ui->toolAlignLeft);
    m_pJustify->addButton(ui->toolAlignRight);

    m_pTextColorModel = new TextStyleColorModel(this);
    ui->comboTextColor->setModel(m_pTextColorModel);

    m_pTextBackModel = new TextStyleBackModel(this);
    ui->comboTextBack->setModel(m_pTextBackModel);

    //loadTestForm();
}

TextStyleDlg::~TextStyleDlg()
{
    delete ui;
}

void TextStyleDlg::loadTestForm()
{
    /*QFile f(":/ui/TextStyleTest.ui");

    if (f.open(QIODevice::ReadOnly))
    {
        QUiLoader loader;
        QMdiSubWindow *wnd = new QMdiSubWindow(this, Qt::CustomizeWindowHint | Qt::WindowTitleHint);

        QWidget *widget = loader.load(&f);
        wnd->setWidget(widget);
        ui->mdiArea->addSubWindow(wnd);
    }*/
}
