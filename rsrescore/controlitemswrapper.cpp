#include "controlitemswrapper.h"
#include <QMetaProperty>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>

ControlItemsWrapper::ControlItemsWrapper(QObject *parent)
    : CustomRectItem{},
    m_propertyModel(nullptr),
    m_inMacro(false)
{

}

ControlItemsWrapper::~ControlItemsWrapper()
{
}

void ControlItemsWrapper::connectControlItemSignals(ControlItem *item)
{
    const QMetaObject *meta = metaObject();
    const QMetaObject *itemMeta = item->metaObject();

    for (int i = meta->methodOffset(); i < meta->methodCount(); ++i)
    {
        QMetaMethod signal = meta->method(i);
        if (signal.methodType() == QMetaMethod::Signal)
        {
            QString signalName = QString::fromLatin1(signal.name());

            int itemSignalIndex = itemMeta->indexOfSignal(
                QMetaObject::normalizedSignature(signalName.toLatin1() + "()"));

            if (itemSignalIndex != -1)
            {
                QMetaMethod itemSignal = itemMeta->method(itemSignalIndex);
                QObject::connect(item, itemSignal, this, signal);
            }
        }
    }
}

void ControlItemsWrapper::disconnectControlItemSignals(ControlItem *item)
{
    // Автоматически отключаем все сигналы
    const QMetaObject *meta = metaObject();
    const QMetaObject *itemMeta = item->metaObject();

    for (int i = meta->methodOffset(); i < meta->methodCount(); ++i)
    {
        QMetaMethod signal = meta->method(i);
        if (signal.methodType() == QMetaMethod::Signal)
        {
            QString signalName = QString::fromLatin1(signal.name());

            int itemSignalIndex = itemMeta->indexOfSignal(
                QMetaObject::normalizedSignature(signalName.toLatin1() + "()"));

            if (itemSignalIndex != -1)
            {
                QMetaMethod itemSignal = itemMeta->method(itemSignalIndex);
                QObject::disconnect(item, itemSignal, this, signal);
            }
        }
    }
}

void ControlItemsWrapper::addControlItem(ControlItem *item)
{
    PropertyModel *model = propertyModel();
    if (item && !m_controlItems.contains(item))
    {
        connectControlItemSignals(item);
        m_controlItems.append(item);
    }

    model->reset();
}

void ControlItemsWrapper::addControlItems(const QVector<ControlItem*> &items)
{
    PropertyModel *model = propertyModel();
    for (ControlItem *item : qAsConst(items))
        addControlItem(item);

    model->reset();
}

QVariant ControlItemsWrapper::userAction(const qint32 &action, const QVariant &param)
{
    if (m_controlItems.isEmpty())
        return QVariant();

    return m_controlItems.first()->userAction(action, param);
}

void ControlItemsWrapper::clearControlItems()
{
    PropertyModel *model = propertyModel();

    for (ControlItem *item : qAsConst(m_controlItems))
        disconnectControlItemSignals(item);

    m_controlItems.clear();
    model->reset();
}

const QVector<ControlItem*> &ControlItemsWrapper::controlItems() const
{
    return m_controlItems;
}

bool ControlItemsWrapper::hasUniformValue(const char *propertyName) const
{
    if (m_controlItems.isEmpty())
        return true;

    const QMetaObject *meta = m_controlItems.first()->metaObject();
    int propIndex = meta->indexOfProperty(propertyName);
    if (propIndex == -1)
        return false;

    QMetaProperty prop = meta->property(propIndex);
    QVariant firstValue = prop.read(m_controlItems.first());

    for (int i = 1; i < m_controlItems.size(); ++i)
    {
        QVariant currentValue = prop.read(m_controlItems[i]);

        if (currentValue != firstValue)
            return false;
    }

    return true;
}

void ControlItemsWrapper::emitAll()
{
    emit fieldTypeChanged();
    emit dataTypeChanged();
    emit dataLengthChanged();
    emit lengthChanged();
    emit linesChanged();
    emit signsChanged();
    emit fdmChanged();
    emit isTextChanged();
    emit controlNameChanged();
    emit controlName2Changed();
    emit valueTemplateChanged();
    emit toolTipChanged();
    emit controlStyleChanged();
    emit controlGroupChanged();
    emit helpPageChanged();
    emit controlFlagsChanged();
    emit tabOrderChanged();
    emit tabNoTabStopChanged();
}

// Реализация геттеров (возвращают значение первого объекта)
ControlItem::FieldType ControlItemsWrapper::fieldType() const
{
    return m_controlItems.isEmpty() ? ControlItem::FET : m_controlItems.first()->fieldType();
}

ControlItem::DataType ControlItemsWrapper::dataType() const
{
    return m_controlItems.isEmpty() ? ControlItem::INT16 : m_controlItems.first()->dataType();
}

quint16 ControlItemsWrapper::dataLength() const
{
    return m_controlItems.isEmpty() ? 0 : m_controlItems.first()->dataLength();
}

quint16 ControlItemsWrapper::length() const
{
    return m_controlItems.isEmpty() ? 0 : m_controlItems.first()->length();
}

quint16 ControlItemsWrapper::lines() const
{
    return m_controlItems.isEmpty() ? 0 : m_controlItems.first()->lines();
}

quint16 ControlItemsWrapper::signs() const
{
    return m_controlItems.isEmpty() ? 0 : m_controlItems.first()->signs();
}

bool ControlItemsWrapper::fdm() const
{
    return m_controlItems.isEmpty() ? false : m_controlItems.first()->fdm();
}

bool ControlItemsWrapper::isText() const
{
    return m_controlItems.isEmpty() ? false : m_controlItems.first()->isText();
}

QString ControlItemsWrapper::controlName() const
{
    return m_controlItems.isEmpty() ? QString() : m_controlItems.first()->controlName();
}

QString ControlItemsWrapper::controlName2() const
{
    return m_controlItems.isEmpty() ? QString() : m_controlItems.first()->controlName2();
}

QString ControlItemsWrapper::valueTemplate() const
{
    return m_controlItems.isEmpty() ? QString() : m_controlItems.first()->valueTemplate();
}

QString ControlItemsWrapper::toolTip() const
{
    return m_controlItems.isEmpty() ? QString() : m_controlItems.first()->toolTip();
}

ResStyle::PanelStyle ControlItemsWrapper::controlStyle() const
{
    return m_controlItems.isEmpty() ? ResStyle::SCOM : m_controlItems.first()->controlStyle();
}

quint16 ControlItemsWrapper::controlGroup() const
{
    return m_controlItems.isEmpty() ? 0 : m_controlItems.first()->controlGroup();
}

quint16 ControlItemsWrapper::helpPage() const
{
    return m_controlItems.isEmpty() ? 0 : m_controlItems.first()->helpPage();
}

ControlItem::ControlFlags ControlItemsWrapper::controlFlags() const
{
    return m_controlItems.isEmpty() ? ControlItem::ControlFlags() : (ControlItem::ControlFlags)m_controlItems.first()->controlFlags();
}

ControTabOrder ControlItemsWrapper::tabOrder() const
{
    return m_controlItems.isEmpty() ? ControTabOrder() : m_controlItems.first()->tabOrder();
}

bool ControlItemsWrapper::noTabStop() const
{
    return m_controlItems.isEmpty() ? false : m_controlItems.first()->noTabStop();
}

bool ControlItemsWrapper::listSelect() const
{
    return m_controlItems.isEmpty() ? false : m_controlItems.first()->listSelect();
}

// Реализация сеттеров (устанавливают значения всем объектам)
void ControlItemsWrapper::setNoTabStop(const bool &val)
{
    if (m_controlItems.isEmpty())
        return;

    if (m_controlItems.size() > 1 && undoStack() && !m_inMacro)
    {
        undoStack()->beginMacro(tr("Изменение режима обхода"));
        m_inMacro = true;
    }

    for (ControlItem *item : qAsConst(m_controlItems))
        item->setNoTabStop(val);

    if (m_inMacro && undoStack())
    {
        undoStack()->endMacro();
        m_inMacro = false;
    }

    emit controlFlagsChanged();
    emit tabNoTabStopChanged();
}

void ControlItemsWrapper::setListSelect(const bool &val)
{
    if (m_controlItems.isEmpty())
        return;

    if (m_controlItems.size() > 1 && undoStack() && !m_inMacro)
    {
        undoStack()->beginMacro(tr("Изменение режима выбора из списка"));
        m_inMacro = true;
    }

    for (ControlItem *item : qAsConst(m_controlItems))
        item->setListSelect(val);

    if (m_inMacro && undoStack())
    {
        undoStack()->endMacro();
        m_inMacro = false;
    }

    emit controlFlagsChanged();
    emit listSelectChanged();
}

void ControlItemsWrapper::setFieldType(ControlItem::FieldType val)
{
    if (m_controlItems.isEmpty())
        return;

    if (m_controlItems.size() > 1 && undoStack() && !m_inMacro)
    {
        undoStack()->beginMacro(tr("Изменение типа поля"));
        m_inMacro = true;
    }

    for (ControlItem *item : qAsConst(m_controlItems))
        item->setFieldType(val);

    if (m_inMacro && undoStack())
    {
        undoStack()->endMacro();
        m_inMacro = false;
    }

    emit fieldTypeChanged();
}

void ControlItemsWrapper::setDataType(ControlItem::DataType val)
{
    if (m_controlItems.isEmpty())
        return;

    if (m_controlItems.size() > 1 && undoStack() && !m_inMacro)
    {
        undoStack()->beginMacro(tr("Изменение типа значения"));
        m_inMacro = true;
    }

    for (ControlItem *item : qAsConst(m_controlItems))
        item->setDataType(val);

    if (m_inMacro && undoStack())
    {
        undoStack()->endMacro();
        m_inMacro = false;
    }

    emit dataTypeChanged();
}

void ControlItemsWrapper::setDataLength(quint16 val)
{
    if (m_controlItems.isEmpty())
        return;

    if (m_controlItems.size() > 1 && undoStack() && !m_inMacro)
    {
        undoStack()->beginMacro(tr("Изменение количества знаков"));
        m_inMacro = true;
    }

    for (ControlItem *item : qAsConst(m_controlItems))
        item->setDataLength(val);

    if (m_inMacro && undoStack())
    {
        undoStack()->endMacro();
        m_inMacro = false;
    }

    emit dataLengthChanged();
}

void ControlItemsWrapper::setSigns(quint16 val)
{
    if (m_controlItems.isEmpty())
        return;

    if (m_controlItems.size() > 1 && undoStack() && !m_inMacro)
    {
        undoStack()->beginMacro(tr("Изменение знаков"));
        m_inMacro = true;
    }

    for (ControlItem *item : qAsConst(m_controlItems))
        item->setSigns(val);

    if (m_inMacro && undoStack())
    {
        undoStack()->endMacro();
        m_inMacro = false;
    }

    emit signsChanged();
}

void ControlItemsWrapper::setFdm(bool val)
{
    if (m_controlItems.isEmpty())
        return;

    if (m_controlItems.size() > 1 && undoStack() && !m_inMacro)
    {
        undoStack()->beginMacro(tr("Изменение признака FDM"));
        m_inMacro = true;
    }

    for (ControlItem *item : qAsConst(m_controlItems))
        item->setFdm(val);

    if (m_inMacro && undoStack())
    {
        undoStack()->endMacro();
        m_inMacro = false;
    }

    emit fdmChanged();
}

void ControlItemsWrapper::setIsText(bool val)
{
    if (m_controlItems.isEmpty())
        return;

    if (m_controlItems.size() > 1 && undoStack() && !m_inMacro)
    {
        undoStack()->beginMacro(tr("Изменение признака текста"));
        m_inMacro = true;
    }

    for (ControlItem *item : qAsConst(m_controlItems))
        item->setIsText(val);

    if (m_inMacro && undoStack())
    {
        undoStack()->endMacro();
        m_inMacro = false;
    }

    emit isTextChanged();
}

void ControlItemsWrapper::setControlName(const QString &val)
{
    if (m_controlItems.isEmpty())
        return;

    if (m_controlItems.size() > 1 && undoStack() && !m_inMacro)
    {
        undoStack()->beginMacro(tr("Изменение наименования поля"));
        m_inMacro = true;
    }

    for (ControlItem *item : qAsConst(m_controlItems))
        item->setControlName(val);

    if (m_inMacro && undoStack())
    {
        undoStack()->endMacro();
        m_inMacro = false;
    }

    emit controlNameChanged();
}

void ControlItemsWrapper::setControlName2(const QString &val)
{
    if (m_controlItems.isEmpty())
        return;

    if (m_controlItems.size() > 1 && undoStack() && !m_inMacro)
    {
        undoStack()->beginMacro(tr("Изменение имени/текста 2"));
        m_inMacro = true;
    }

    for (ControlItem *item : qAsConst(m_controlItems))
        item->setControlName2(val);

    if (m_inMacro && undoStack())
    {
        undoStack()->endMacro();
        m_inMacro = false;
    }

    emit controlName2Changed();
}

void ControlItemsWrapper::setValueTemplate(const QString &val)
{
    if (m_controlItems.isEmpty())
        return;

    if (m_controlItems.size() > 1 && undoStack() && !m_inMacro)
    {
        undoStack()->beginMacro(tr("Изменение шаблона значения"));
        m_inMacro = true;
    }

    for (ControlItem *item : qAsConst(m_controlItems))
        item->setValueTemplate(val);

    if (m_inMacro && undoStack())
    {
        undoStack()->endMacro();
        m_inMacro = false;
    }

    emit valueTemplateChanged();
}

void ControlItemsWrapper::setToolTip(const QString &val)
{
    if (m_controlItems.isEmpty())
        return;

    if (m_controlItems.size() > 1 && undoStack() && !m_inMacro)
    {
        undoStack()->beginMacro(tr("Изменение подсказки"));
        m_inMacro = true;
    }

    for (ControlItem *item : qAsConst(m_controlItems))
        item->setToolTip(val);

    if (m_inMacro && undoStack())
    {
        undoStack()->endMacro();
        m_inMacro = false;
    }

    emit toolTipChanged();
}

void ControlItemsWrapper::setControlStyle(ResStyle::PanelStyle val)
{
    if (m_controlItems.isEmpty())
        return;

    if (m_controlItems.size() > 1 && undoStack() && !m_inMacro)
    {
        undoStack()->beginMacro(tr("Изменение цвета"));
        m_inMacro = true;
    }

    for (ControlItem *item : qAsConst(m_controlItems))
        item->setControlStyle(val);

    if (m_inMacro && undoStack())
    {
        undoStack()->endMacro();
        m_inMacro = false;
    }

    emit controlStyleChanged();
}

void ControlItemsWrapper::setControlGroup(quint16 val)
{
    if (m_controlItems.isEmpty())
        return;

    if (m_controlItems.size() > 1 && undoStack() && !m_inMacro)
    {
        undoStack()->beginMacro(tr("Изменение группы"));
        m_inMacro = true;
    }

    for (ControlItem *item : qAsConst(m_controlItems))
        item->setControlGroup(val);

    if (m_inMacro && undoStack())
    {
        undoStack()->endMacro();
        m_inMacro = false;
    }

    emit controlGroupChanged();
}

void ControlItemsWrapper::setHelpPage(quint16 val)
{
    if (m_controlItems.isEmpty())
        return;

    if (m_controlItems.size() > 1 && undoStack() && !m_inMacro)
    {
        undoStack()->beginMacro(tr("Изменение страницы помощи"));
        m_inMacro = true;
    }

    for (ControlItem *item : qAsConst(m_controlItems))
        item->setHelpPage(val);

    if (m_inMacro && undoStack())
    {
        undoStack()->endMacro();
        m_inMacro = false;
    }

    emit helpPageChanged();
}

void ControlItemsWrapper::setControlFlags(ControlItem::ControlFlags val)
{
    if (m_controlItems.isEmpty())
        return;

    if (m_controlItems.size() > 1 && undoStack() && !m_inMacro)
    {
        undoStack()->beginMacro(tr("Изменение дополнительных признаков"));
        m_inMacro = true;
    }

    for (ControlItem *item : qAsConst(m_controlItems))
        item->setControlFlags(val);

    if (m_inMacro && undoStack())
    {
        undoStack()->endMacro();
        m_inMacro = false;
    }

    emit controlFlagsChanged();
}

void ControlItemsWrapper::setTabOrder(ControTabOrder val)
{
    if (m_controlItems.isEmpty())
        return;

    if (m_controlItems.size() > 1 && undoStack() && !m_inMacro)
    {
        undoStack()->beginMacro(tr("Изменение порядка полей"));
        m_inMacro = true;
    }

    for (ControlItem *item : qAsConst(m_controlItems))
        item->setTabOrder(val);

    if (m_inMacro && undoStack())
    {
        undoStack()->endMacro();
        m_inMacro = false;
    }

    emit tabOrderChanged();
}
