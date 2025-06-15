#include "propertywidgetmapper.h"
#include <QDebug>
#include <QMetaProperty>
#include <QAction>
#include <QObject>

class ObjectMapperPrivate
{
    Q_DECLARE_PUBLIC(ObjectMapper)
public:
    ObjectMapperPrivate(ObjectMapper *obj) :
        q_ptr(obj)
    {
    }

    QObject *object;
    QObject *reciever;
    QByteArray property;

    ObjectMapper *q_ptr;
};

ObjectMapper::ObjectMapper(QObject *parent) :
    QObject(parent),
    d_ptr(new ObjectMapperPrivate(this))
{

}

void ObjectMapper::setChecked()
{
    Q_D(ObjectMapper);
    QAction *action = qobject_cast<QAction*>(d->reciever);
    QVariant value = d->object->property(d->property);

    if (action)
        action->setChecked(value.toBool());
}
// -----------------------------------------------------------------------

class PropertyWidgetMapperPrivate
{
    Q_DECLARE_PUBLIC(PropertyWidgetMapper)
public:
    PropertyWidgetMapperPrivate(PropertyWidgetMapper *obj) :
        q_ptr(obj)
    {
    }

    QObject *addMap(const QByteArray &property, QObject *object, QObject *reciever)
    {
        Q_Q(PropertyWidgetMapper);
        ObjectMapper *mapobj = new ObjectMapper(q);
        mapobj->d_ptr->object = object;
        mapobj->d_ptr->property = property;
        mapobj->d_ptr->reciever = reciever;
        m_Mapper.append(mapobj);

        return mapobj;
    }

    QList<ObjectMapper*> m_Mapper;
    PropertyWidgetMapper *q_ptr;
};

PropertyWidgetMapper::PropertyWidgetMapper(QObject *parent)
    : QObject{parent},
    d_ptr(new PropertyWidgetMapperPrivate(this))
{

}

PropertyWidgetMapper::~PropertyWidgetMapper()
{
    delete d_ptr;
}

void PropertyWidgetMapper::setChecked()
{
    QObject *obj = sender();
}

bool PropertyWidgetMapper::bind(QObject *source, const char *property, QAction *action)
{
    Q_D(PropertyWidgetMapper);
    if (!source || !property || !action)
        return false;

    const QMetaObject *meta = source->metaObject();
    int propIndex = meta->indexOfProperty(property);
    if (propIndex == -1)
    {
        qWarning() << "Property" << property << "not found in" << source;
        return false;
    }

    QMetaProperty sourceProp = meta->property(propIndex);

    // Ищем свойство "checked" в QAction
    QMetaProperty targetProp = findTargetProperty(action, "checked");
    if (!targetProp.isValid())
    {
        qWarning() << "QAction does not have a suitable property (checked or USER flag)";
        return false;
    }

    if (sourceProp.type() == QVariant::Bool)
    {
        // Устанавливаем начальное значение
        targetProp.write(action, sourceProp.read(source).toBool());

        // Обновляем action при изменении свойства
        QString signalmethod = QString("2%1").arg(sourceProp.notifySignal().methodSignature().data());
        //connect(source, signalmethod.toLocal8Bit().data(), this, SLOT(setChecked()));
        QObject *mapobj = d->addMap(property, source, action);
        connect(source, signalmethod.toLocal8Bit().data(), mapobj, SLOT(setChecked()));

        // Обновляем свойство при изменении action
        connect(action, &QAction::toggled, [=](bool checked)
        {
            sourceProp.write(source, checked);
        });

        return true;
    }
    else
    {
        qWarning() << "Property type is not bool";
        return false;
    }
}

bool PropertyWidgetMapper::bind(QObject *source, const char *property, QWidget *widget, const char *widgetProperty)
{
    if (!source || !property || !widget)
        return false;

    /*const QMetaObject *meta = source->metaObject();
    int propIndex = meta->indexOfProperty(property);
    if (propIndex == -1)
    {
        qWarning() << "Property" << property << "not found in" << source;
        return false;
    }

    QMetaProperty sourceProp = meta->property(propIndex);

    // Ищем свойство в виджете (если widgetProperty == nullptr, ищем USER-свойство или с таким же именем)
    QByteArray preferredName = widgetProperty ? QByteArray(widgetProperty) : QByteArray(property);
    QMetaProperty targetProp = findTargetProperty(widget, preferredName);
    if (!targetProp.isValid())
    {
        qWarning() << "Widget does not have a suitable property:" << preferredName;
        return false;
    }

    // Проверяем совместимость типов
    if (sourceProp.type() != targetProp.type())
    {
        qWarning() << "Property types do not match:" << sourceProp.typeName() << "!=" << targetProp.typeName();
        return false;
    }

    // Обновляем виджет при изменении свойства
    //connect(source, &QObject::destroyed, this, &PropertyWidgetMapper::clearBinding);
    connect(source, property, [=]() {
        QVariant value = sourceProp.read(source);
        targetProp.write(widget, value);
    });
    connect(source, sourceProp.notifySignal().methodSignature(), widget, targetProp.);

    // Если виджет может уведомлять об изменении (например, textChanged), обновляем источник
    QByteArray changedSignal = targetProp.name() + "Changed";
    if (widget->metaObject()->indexOfSignal(changedSignal + "(QVariant)") != -1)
    {
        connect(widget, changedSignal + "(QVariant)", [=](const QVariant &value) {
            sourceProp.write(source, value);
        });
    }
    else if (widget->metaObject()->indexOfSignal(changedSignal + "()") != -1)
    {
        connect(widget, changedSignal + "()", [=]() {
            QVariant value = targetProp.read(widget);
            sourceProp.write(source, value);
        });
    }

    // Устанавливаем начальное значение
    targetProp.write(widget, sourceProp.read(source));*/
    return true;
}

QMetaProperty PropertyWidgetMapper::findTargetProperty(QObject *target, const QByteArray &preferredName) const
{
    const QMetaObject *meta = target->metaObject();

    // Сначала ищем свойство с USER-флагом
    for (int i = 0; i < meta->propertyCount(); ++i)
    {
        QMetaProperty prop = meta->property(i);

        if (prop.isUser())
            return prop;
    }

    // Если не нашли, ищем свойство с указанным именем
    int propIndex = meta->indexOfProperty(preferredName);
    if (propIndex != -1)
        return meta->property(propIndex);

    return QMetaProperty(); // Невалидное свойство
}
