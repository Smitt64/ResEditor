#ifndef TEXTSTYLEDLG_H
#define TEXTSTYLEDLG_H

#include "propertymodel/enumlistmodel.h"
#include <QDialog>

namespace Ui {
class TextStyleDlg;
}

class TextStyleColorModel : public EnumListModel
{
public:
    TextStyleColorModel(QObject *parent = nullptr);
    virtual ~TextStyleColorModel();

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
};

class TextStyleBackModel : public EnumListModel
{
public:
    TextStyleBackModel(QObject *parent = nullptr);
    virtual ~TextStyleBackModel();

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
};

class QButtonGroup;
class EnumListModel;
class TextStyleDlg : public QDialog
{
    Q_OBJECT

public:
    explicit TextStyleDlg(QWidget *parent = nullptr);
    ~TextStyleDlg();

private:
    void loadTestForm();
    Ui::TextStyleDlg *ui;

    QButtonGroup *m_pJustify;

    TextStyleColorModel *m_pTextColorModel;
    TextStyleBackModel *m_pTextBackModel;
};

#endif // TEXTSTYLEDLG_H
