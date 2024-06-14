#ifndef PANELSTRUCTMODEL_H
#define PANELSTRUCTMODEL_H

#include <QAbstractItemModel>
#include <QObject>
#include <memory>

class TreeItem;
class CustomRectItem;
class QGraphicsItem;
class PanelStructModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit PanelStructModel(CustomRectItem *item, QObject *parent = nullptr);
    virtual ~PanelStructModel();

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = {}) const override;

    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;

private slots:
    void childAdded(CustomRectItem *item);

private:
    std::unique_ptr<TreeItem> rootItem;
};

#endif // PANELSTRUCTMODEL_H
