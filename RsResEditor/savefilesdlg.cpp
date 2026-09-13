#include "savefilesdlg.h"
#include "ui_savefilesdlg.h"
#include <QStandardItemModel>
#include <QPushButton>

FileInfo::FileInfo(int id, const QString& name,
                   const QString& category, const QIcon& icon)
    : m_id(id), m_name(name), m_category(category), m_icon(icon)
{
}

int FileInfo::id() const { return m_id; }
QString FileInfo::name() const { return m_name; }
QString FileInfo::category() const { return m_category; }
QIcon FileInfo::icon() const { return m_icon; }

// --------------------------------------------------------

SaveFilesDlg::SaveFilesDlg(const QList<FileInfo> &files, QWidget *parent)
    : QDialog(parent),
    ui(new Ui::SaveFilesDlg),
    m_SaveModeResult(0),
    m_files(files)
{
    ui->setupUi(this);
    setWindowIcon(QIcon::fromTheme("SaveFileDialog"));

    m_saveButton = ui->buttonBox->button(QDialogButtonBox::Save);
    m_saveAllButton = ui->buttonBox->button(QDialogButtonBox::SaveAll);

    m_noButton = ui->buttonBox->addButton(tr("Не сохранять"), QDialogButtonBox::ActionRole);
    m_noAllButton = ui->buttonBox->addButton(tr("Не сохранять все"), QDialogButtonBox::ActionRole);
    ui->buttonBox->addButton(QDialogButtonBox::Cancel);

    m_model = new QStandardItemModel(this);
    m_model->setHorizontalHeaderLabels({"Category", "File Name"});

    for (const FileInfo& file : std::as_const(m_files))
    {
        QList<QStandardItem*> rowItems;

        QStandardItem* categoryItem = new QStandardItem(file.category());
        QStandardItem* nameItem = new QStandardItem(file.name());
        categoryItem->setIcon(file.icon());
        nameItem->setData(file.id(), Qt::UserRole + 1);

        rowItems << categoryItem << nameItem;
        m_model->appendRow(rowItems);
    }

    ui->treeView->setModel(m_model);

    QItemSelection selection(m_model->index(0, 0), m_model->index(m_model->rowCount() - 1, m_model->columnCount() - 1));
    ui->treeView->selectionModel()->select(selection, QItemSelectionModel::Select | QItemSelectionModel::Rows);

    //connect(m_saveButton, &QPushButton::clicked, this, &SaveFilesDialog::onSaveClicked);
    //connect(m_saveAllButton, &QPushButton::clicked, this, &SaveFilesDialog::onSaveAllClicked);
    //connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(m_noButton, &QPushButton::clicked, this, &SaveFilesDlg::onNoClicked);
    connect(m_noAllButton, &QPushButton::clicked, this, &SaveFilesDlg::onNoClicked);

    connect(m_saveButton, &QPushButton::clicked, this, &SaveFilesDlg::onSaveClicked);
    connect(m_noAllButton, &QPushButton::clicked, this, &SaveFilesDlg::onSaveClicked);
    connect(ui->treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &SaveFilesDlg::onSelectionChanged);

    updateButtonVisibility();
}

SaveFilesDlg::~SaveFilesDlg()
{
    delete ui;
}

QList<int> SaveFilesDlg::selectedIds() const
{
    QList<int> ids;
    QModelIndexList selected = ui->treeView->selectionModel()->selectedRows();

    for (const QModelIndex& index : std::as_const(selected))
    {
        QStandardItem* item = m_model->itemFromIndex(index.sibling(index.row(), 1));

        if (item)
            ids.append(item->data(Qt::UserRole + 1).toInt());
    }

    return ids;
}

void SaveFilesDlg::onNoClicked()
{
    m_SaveModeResult = ResultDiscard;
    done(ResultDiscard);
}

void SaveFilesDlg::onSaveClicked()
{
    m_SaveModeResult = ResultSave;
    done(ResultSave);
}

int SaveFilesDlg::saveModeResult() const
{
    return m_SaveModeResult;
}

void SaveFilesDlg::onSelectionChanged()
{
    bool allSelected = true;
    for (int i = 0; i < m_model->rowCount(); ++i)
    {
        if (!ui->treeView->selectionModel()->isSelected(m_model->index(i, 0)))
        {
            allSelected = false;
            break;
        }
    }

    if (m_allSelected != allSelected)
    {
        m_allSelected = allSelected;
        updateButtonVisibility();
    }
}

void SaveFilesDlg::updateButtonVisibility()
{
    bool hasSelection = !ui->treeView->selectionModel()->selectedRows().isEmpty();

    if (hasSelection)
    {
        if (m_model->rowCount() > 1)
        {
            m_saveButton->setVisible(!m_allSelected);
            m_saveAllButton->setVisible(m_allSelected);
        }
        else
        {
            m_saveButton->setVisible(true);
            m_saveAllButton->setVisible(false);
        }
    }

    if (m_model->rowCount() > 1)
    {
        m_saveButton->setEnabled(hasSelection && !m_allSelected);
        m_saveAllButton->setEnabled(hasSelection && m_allSelected);
    }
    else
        m_saveButton->setEnabled(hasSelection);

    if (m_model->rowCount() > 1)
        m_noButton->setVisible(false);
    else
        m_noAllButton->setVisible(false);
}
