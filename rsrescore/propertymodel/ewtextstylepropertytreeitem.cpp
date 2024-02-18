#include "ewtextstylepropertytreeitem.h"
#include "customrectitem.h"
#include "styles/extextstyle.h"
#include "propertymodel/flagpropertytreeitem.h"
#include "enumlistmodel.h"
#include "textstyledlg.h"
#include <QMetaObject>
#include <QMetaProperty>
#include <QMetaEnum>
#include <QMetaType>
#include <QJsonArray>
#include <QPushButton>
#include <QComboBox>

EwTextStylePropertyTreeItem::EwTextStylePropertyTreeItem(QObject *rectItem, QObject *parent)
    : PropertyTreeItem{rectItem, parent},
      m_pStyle(nullptr),
      m_pFormat(nullptr),
      m_pAlignEnumModel(nullptr),
      m_pTextColorEnumModel(nullptr)
{
    m_SubType = SubTypeRoot;

    m_pAlign = new EwTextStylePropertyTreeItem(SubTypeAlign, nullptr, this);
    m_pAlign->setPropertyAlias(tr("Выравнивание"));

    m_pTextcolor = new EwTextStylePropertyTreeItem(SubTypeTextColor, nullptr, this);
    m_pTextcolor->setPropertyAlias(tr("Цвет текста"));

    m_pTextBack = new EwTextStylePropertyTreeItem(SubTypeTextBack, nullptr, this);
    m_pTextBack->setPropertyAlias(tr("Цвет фона"));

    m_pFormat = new FlagPropertyTreeItem(nullptr, FlagPropertyTreeItem::SubTypeRoot, this);

    /*m_EwChilds.append(pAlign);
    m_EwChilds.append(m_pFormat);*/
    appendChild(m_pAlign);
    appendChild(m_pFormat);
    appendChild(m_pTextcolor);
    appendChild(m_pTextBack);

    connect(rectItem, SIGNAL(textStyleChanged()), this, SLOT(rectItemStyleChanged()));
}

EwTextStylePropertyTreeItem::EwTextStylePropertyTreeItem(const NodeSubType &type, CustomRectItem *rectItem, QObject *parent)
    : PropertyTreeItem{rectItem, parent},
      m_SubType(type),
      m_pStyle(nullptr)
{
    //EwTextStylePropertyTreeItem *prnt = qobject_cast<EwTextStylePropertyTreeItem*>(parent);
    //setPropertyName(prnt->propertyName());
}

EwTextStylePropertyTreeItem::~EwTextStylePropertyTreeItem()
{
    if (m_pStyle)
        delete m_pStyle;
}

void EwTextStylePropertyTreeItem::initModels()
{
    const QMetaObject *metaobject = m_pItem->metaObject();
    int propIndex = metaobject->indexOfProperty(m_PropertyName.toLocal8Bit().data());

    if (propIndex >= 0)
    {
        QMetaProperty prop = metaobject->property(propIndex);

        if (prop.userType() == QMetaType::type("EwTextStyle"))
        {
            QVariant variantvalue = m_pItem->property(m_PropertyName.toLocal8Bit());
            EwTextStyle value = variantvalue.value<EwTextStyle>();

            m_pStyle = new EwTextStyle();
            m_pStyle->fromOther(value);

            m_pAlign->setObject(m_pStyle);
            m_pAlign->setPropertyName("align");
            m_pAlign->setGroup(group());
            m_pAlign->m_pStyle = m_pStyle;

            m_pTextcolor->setObject(m_pStyle);
            m_pTextcolor->setPropertyName("textColor");
            m_pTextcolor->setGroup(group());
            m_pTextcolor->m_pStyle = m_pStyle;

            m_pTextBack->setObject(m_pStyle);
            m_pTextBack->setPropertyName("backColor");
            m_pTextBack->setGroup(group());
            m_pTextBack->m_pStyle = m_pStyle;

            m_pFormat->setObject(m_pStyle);
            m_pFormat->setGroup(group());
            m_pFormat->setPropertyName("format");
            m_pFormat->setPropertyAlias(tr("Формат"));

            const QMetaObject *stylemetaobject = m_pStyle->metaObject();
            int alignIndex = stylemetaobject->indexOfEnumerator("TextAlign");
            int textColorIndex = stylemetaobject->indexOfEnumerator("TextColor");
            int textBackIndex = stylemetaobject->indexOfEnumerator("BackColor");

            m_pAlignEnumModel->setMetaEnum(stylemetaobject->enumerator(alignIndex));
            m_pTextColorEnumModel->setMetaEnum(stylemetaobject->enumerator(textColorIndex));
            m_pTextBackEnumModel->setMetaEnum(stylemetaobject->enumerator(textBackIndex));

            connect(m_pStyle, &EwTextStyle::alignChanged, this, &EwTextStylePropertyTreeItem::propertyChanged);
            connect(m_pStyle, &EwTextStyle::formatChanged, this, &EwTextStylePropertyTreeItem::propertyChanged);
            connect(m_pStyle, &EwTextStyle::textColorChanged, this, &EwTextStylePropertyTreeItem::propertyChanged);
            connect(m_pStyle, &EwTextStyle::textBackChanged, this, &EwTextStylePropertyTreeItem::propertyChanged);
        }
    }

    for (PropertyTreeItem *item : qAsConst(m_EwChilds))
    {
        item->setObject(m_pStyle);
        item->setGroup(group());
    }
}

void EwTextStylePropertyTreeItem::initFromJson(const QJsonObject &obj)
{
    QJsonArray alignenum = obj["alignenum"].toArray();
    QJsonArray formatenum = obj["formatenum"].toArray();

    m_pAlignEnumModel = new EnumListModel(this);
    m_pAlignEnumModel->loadFromJsonArray(alignenum);

    m_pTextColorEnumModel = new TextStyleColorModel(this);
    m_pTextBackEnumModel = new TextStyleBackModel(this);

    m_pFormat->loadEnumAlias(formatenum);
    initModels();
}

void EwTextStylePropertyTreeItem::setPropertyName(const QString &name)
{
    PropertyTreeItem::setPropertyName(name);

    if (m_SubType != SubTypeRoot)
        return;


}

void EwTextStylePropertyTreeItem::setData(const QVariant &value)
{
    if (!m_pItem)
        return;
}

QVariant EwTextStylePropertyTreeItem::data(const int &role) const
{
    const QMetaObject *metaobject = m_pItem->metaObject();
    int propIndex = metaobject->indexOfProperty(m_PropertyName.toLocal8Bit().data());

    if (propIndex < 0)
        return QVariant();

    if (role == Qt::EditRole || role == Qt::DisplayRole)
    {
        if (m_SubType == SubTypeAlign)
        {
            EwTextStylePropertyTreeItem *pParent = qobject_cast<EwTextStylePropertyTreeItem*>(parent());

            if (role == Qt::EditRole)
                return m_pStyle->align();
            else if (role == Qt::DisplayRole)
                return pParent->m_pAlignEnumModel->valueToAlias(m_pStyle->align());

        }
        else if (m_SubType == SubTypeTextColor)
        {
            EwTextStylePropertyTreeItem *pParent = qobject_cast<EwTextStylePropertyTreeItem*>(parent());

            if (role == Qt::EditRole)
                return m_pStyle->textColor();
            else if (role == Qt::DisplayRole)
                return pParent->m_pTextColorEnumModel->valueToAlias(m_pStyle->textColor());
        }
        else if (m_SubType == SubTypeTextBack)
        {
            EwTextStylePropertyTreeItem *pParent = qobject_cast<EwTextStylePropertyTreeItem*>(parent());

            if (role == Qt::EditRole)
                return m_pStyle->backColor();
            else if (role == Qt::DisplayRole)
                return pParent->m_pTextBackEnumModel->valueToAlias(m_pStyle->backColor());
        }
    }

    return PropertyTreeItem::data(role);
}

QWidget *EwTextStylePropertyTreeItem::createEditor(QWidget *wparent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *pWidget = nullptr;
    EwTextStylePropertyTreeItem *pParent = qobject_cast<EwTextStylePropertyTreeItem*>(parent());

    if (m_SubType == SubTypeRoot)
    {
        QPushButton *btn = new QPushButton(wparent);
        btn->setText(tr("Изменить стиль"));
        pWidget = btn;

        connect(btn, &QPushButton::clicked, this, &EwTextStylePropertyTreeItem::openTextStyleDialog);
    }
    else if (m_SubType == SubTypeAlign)
    {
        QComboBox *combo = new QComboBox(wparent);
        combo->setModel(pParent->m_pAlignEnumModel);
        pWidget = combo;
    }
    else if (m_SubType == SubTypeTextColor)
    {
        QComboBox *combo = new QComboBox(wparent);
        combo->setModel(pParent->m_pTextColorEnumModel);
        pWidget = combo;
    }
    else if (m_SubType == SubTypeTextBack)
    {
        QComboBox *combo = new QComboBox(wparent);
        combo->setModel(pParent->m_pTextBackEnumModel);
        pWidget = combo;
    }

    return pWidget;
}

bool EwTextStylePropertyTreeItem::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (m_SubType == SubTypeAlign)
    {
        QComboBox *combo = qobject_cast<QComboBox*>(editor);
        combo->setCurrentIndex(m_pStyle->align());
    }
    else if (m_SubType == SubTypeTextColor)
    {
        EwTextStylePropertyTreeItem *pParent = qobject_cast<EwTextStylePropertyTreeItem*>(parent());
        QComboBox *combo = qobject_cast<QComboBox*>(editor);

        int value = pParent->m_pTextColorEnumModel->indexFromValue(m_pStyle->textColor());
        combo->setCurrentIndex(value);
    }
    else if (m_SubType == SubTypeTextBack)
    {
        EwTextStylePropertyTreeItem *pParent = qobject_cast<EwTextStylePropertyTreeItem*>(parent());
        QComboBox *combo = qobject_cast<QComboBox*>(editor);

        int value = pParent->m_pTextBackEnumModel->indexFromValue(m_pStyle->backColor());
        combo->setCurrentIndex(value);
    }

    return true;
}

bool EwTextStylePropertyTreeItem::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    EwTextStylePropertyTreeItem *pParent = qobject_cast<EwTextStylePropertyTreeItem*>(parent());
    if (m_SubType == SubTypeAlign)
    {
        QComboBox *combo = qobject_cast<QComboBox*>(editor);
        QString key = pParent->m_pAlignEnumModel->key(combo->currentText());

        int value = pParent->m_pAlignEnumModel->keyToValue(key.toLocal8Bit());
        m_pStyle->setProperty("align", value);
    }
    else if (m_SubType == SubTypeTextColor)
    {
        QComboBox *combo = qobject_cast<QComboBox*>(editor);
        QString key = pParent->m_pTextColorEnumModel->key(combo->currentText());

        int value = pParent->m_pTextColorEnumModel->keyToValue(key.toLocal8Bit());
        m_pStyle->setProperty("textColor", value);
    }
    else if (m_SubType == SubTypeTextBack)
    {
        QComboBox *combo = qobject_cast<QComboBox*>(editor);
        QString key = pParent->m_pTextBackEnumModel->key(combo->currentText());

        int value = pParent->m_pTextBackEnumModel->keyToValue(key.toLocal8Bit());
        m_pStyle->setProperty("backColor", value);
    }

    return true;
}

void EwTextStylePropertyTreeItem::propertyChanged()
{
    m_pItem->setProperty(m_PropertyName.toLocal8Bit(), QVariant::fromValue(*m_pStyle));
}

void EwTextStylePropertyTreeItem::rectItemStyleChanged()
{
    QVariant var = m_pItem->property(m_PropertyName.toLocal8Bit());
    EwTextStyle Style = var.value<EwTextStyle>();
    m_pStyle->fromOther(Style);

    emit dataChanged();
    for (PropertyTreeItem *item : qAsConst(m_EwChilds))
        emit item->dataChanged();
}

void EwTextStylePropertyTreeItem::openTextStyleDialog()
{
    QWidget *pParent = qobject_cast<QWidget*>(sender());

    TextStyleDlg dlg(pParent);
    dlg.exec();
}
