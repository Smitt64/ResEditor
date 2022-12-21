#ifndef TOOLBOXWIDGETINTERFACE_H
#define TOOLBOXWIDGETINTERFACE_H

#include <QWidget>
// https://code.woboq.org/qt5/qttools/src/designer/src/components/widgetbox/widgetboxtreewidget.h.html#qdesigner_internal::WidgetBoxTreeWidget
class ToolBoxWidgetInterfaceData;
class ToolBoxWidgetInterface : public QWidget
{
    Q_OBJECT
public:
    class Widget
    {
    public:
        enum Type { Default, Custom };
        Widget(const QString &aname = QString(), const QString &xml = QString(),
               const QString &icon_name = QString(), Type atype = Default);
        ~Widget();
        Widget(const Widget &w);
        Widget &operator=(const Widget &w);
        QString name() const;
        void setName(const QString &aname);
        QString domXml() const;
        void setDomXml(const QString &xml);
        QString iconName() const;
        void setIconName(const QString &icon_name);
        Type type() const;
        void setType(Type atype);
        bool isNull() const;

    private:
        QSharedDataPointer<ToolBoxWidgetInterfaceData> m_data;
    };

    class Category
    {
    public:
        enum Type { Default, Scratchpad };
        Category(const QString &aname = QString(), Type atype = Default)
            : m_name(aname), m_type(atype) {}
        QString name() const { return m_name; }
        void setName(const QString &aname) { m_name = aname; }
        int widgetCount() const { return m_widget_list.size(); }
        Widget widget(int idx) const { return m_widget_list.at(idx); }
        void removeWidget(int idx) { m_widget_list.removeAt(idx); }
        void addWidget(const Widget &awidget) { m_widget_list.append(awidget); }
        Type type() const { return m_type; }
        void setType(Type atype) { m_type = atype; }
        bool isNull() const { return m_name.isEmpty(); }

    private:
        QString m_name;
        Type m_type;
        QList<Widget> m_widget_list;
    };
    using CategoryList = QList<Category>;

    ToolBoxWidgetInterface(QWidget *parent = nullptr);
    virtual ~ToolBoxWidgetInterface();

    virtual int categoryCount() const = 0;
    virtual Category category(int cat_idx) const = 0;
    virtual void addCategory(const Category &cat) = 0;
    virtual void removeCategory(int cat_idx) = 0;
    virtual int widgetCount(int cat_idx) const = 0;
    virtual Widget widget(int cat_idx, int wgt_idx) const = 0;
    virtual void addWidget(int cat_idx, const Widget &wgt) = 0;
    virtual void removeWidget(int cat_idx, int wgt_idx) = 0;

    int findOrInsertCategory(const QString &categoryName);

signals:

};

#endif // TOOLBOXWIDGETINTERFACE_H
