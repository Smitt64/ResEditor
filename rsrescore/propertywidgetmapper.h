#ifndef PROPERTYWIDGETMAPPER_H
#define PROPERTYWIDGETMAPPER_H

#include <QObject>

class ObjectMapperPrivate;
class ObjectMapper : public QObject
{
    Q_OBJECT
    friend class PropertyWidgetMapper;
    friend class PropertyWidgetMapperPrivate;
public:
    ObjectMapper(QObject *parent = nullptr);
    virtual ~ObjectMapper() = default;

public slots:
    void setChecked();

private:
    ObjectMapperPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(ObjectMapper);
};

class QAction;
class PropertyWidgetMapperPrivate;
class PropertyWidgetMapper : public QObject
{
    Q_OBJECT
public:
    explicit PropertyWidgetMapper(QObject *parent = nullptr);
    virtual ~PropertyWidgetMapper();

    // Привязка Q_PROPERTY к QAction (например, checked)
    bool bind(QObject *source, const char *property, QAction *action);

    // Привязка Q_PROPERTY к QWidget (если widgetProperty == nullptr, ищем USER-свойство)
    bool bind(QObject *source, const char *property, QWidget *widget, const char *widgetProperty = nullptr);

private slots:
    void setChecked();

private:
    PropertyWidgetMapperPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(PropertyWidgetMapper);

    // Поиск свойства с флагом USER или с заданным именем
    QMetaProperty findTargetProperty(QObject *target, const QByteArray &preferredName) const;
};

#endif // PROPERTYWIDGETMAPPER_H
