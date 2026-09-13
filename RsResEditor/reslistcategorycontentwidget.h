#ifndef RESLISTCATEGORYCONTENTWIDGET_H
#define RESLISTCATEGORYCONTENTWIDGET_H

#include <ribbon/categorycontentwidget.h>

class QMainWindow;
class LbrObjectInterface;
class ResListCategoryContentWidget : public CategoryContentWidget
{
    Q_OBJECT
public:
    ResListCategoryContentWidget(LbrObjectInterface *pLbrObj, QWidget *parent = nullptr);
    virtual ~ResListCategoryContentWidget();

signals:
    void openResource(const QString &name, const int &type);

private slots:
    void onClicked(const QModelIndex &index);

private:
    QMainWindow *m_pContainer;

    int m_type;
    QString m_name;
};

#endif //
