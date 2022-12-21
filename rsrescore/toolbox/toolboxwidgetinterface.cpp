#include "toolboxwidgetinterface.h"

class ToolBoxWidgetInterfaceData : public QSharedData
{
public:
    ToolBoxWidgetInterfaceData(const QString &aname, const QString &xml, const QString &icon_name, ToolBoxWidgetInterface::Widget::Type atype) :
        m_name(aname), m_xml(xml), m_icon_name(icon_name), m_type(atype)
    {

    }

    QString m_name;
    QString m_xml;
    QString m_icon_name;
    ToolBoxWidgetInterface::Widget::Type m_type;
};


ToolBoxWidgetInterface::Widget::Widget(const QString &aname, const QString &xml, const QString &icon_name, Type atype) :
    m_data(new ToolBoxWidgetInterfaceData(aname, xml, icon_name, atype))
{
}

ToolBoxWidgetInterface::Widget::~Widget() = default;
ToolBoxWidgetInterface::Widget::Widget(const Widget &w) :
    m_data(w.m_data)
{
}

ToolBoxWidgetInterface::Widget &ToolBoxWidgetInterface::Widget::operator=(const Widget &rhs)
{
    if (this != &rhs) {
        m_data = rhs.m_data;
    }
    return *this;
}

QString ToolBoxWidgetInterface::Widget::name() const
{
    return m_data->m_name;
}

void ToolBoxWidgetInterface::Widget::setName(const QString &aname)
{
    m_data->m_name = aname;
}

QString ToolBoxWidgetInterface::Widget::domXml() const
{
    return m_data->m_xml;
}

void ToolBoxWidgetInterface::Widget::setDomXml(const QString &xml)
{
    m_data->m_xml = xml;
}

QString ToolBoxWidgetInterface::Widget::iconName() const
{
    return m_data->m_icon_name;
}

void ToolBoxWidgetInterface::Widget::setIconName(const QString &icon_name)
{
    m_data->m_icon_name = icon_name;
}

ToolBoxWidgetInterface::Widget::Type ToolBoxWidgetInterface::Widget::type() const
{
    return m_data->m_type;
}

void ToolBoxWidgetInterface::Widget::setType(Type atype)
{
    m_data->m_type = atype;
}

bool ToolBoxWidgetInterface::Widget::isNull() const
{
    return m_data->m_name.isEmpty();
}

// -----------------------------------------------------------------------------------

ToolBoxWidgetInterface::ToolBoxWidgetInterface(QWidget *parent)
    : QWidget{parent}
{

}

ToolBoxWidgetInterface::~ToolBoxWidgetInterface()
{

}

int ToolBoxWidgetInterface::findOrInsertCategory(const QString &categoryName)
{
    int count = categoryCount();
    for (int index=0; index<count; ++index)
    {
        Category c = category(index);
        if (c.name() == categoryName)
            return index;
    }

    addCategory(Category(categoryName));
    return count;
}
