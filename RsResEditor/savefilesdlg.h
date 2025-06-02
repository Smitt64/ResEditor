#ifndef SAVEFILESDLG_H
#define SAVEFILESDLG_H

#include <QDialog>
#include <QIcon>

namespace Ui {
class SaveFilesDlg;
}

class FileInfo
{
public:
    FileInfo(int id = 0, const QString& name = QString(),
             const QString& category = QString(), const QIcon& icon = QIcon());

    int id() const;
    QString name() const;
    QString category() const;
    QIcon icon() const;

private:
    int m_id;
    QString m_name;
    QString m_category;
    QIcon m_icon;
};

class QStandardItemModel;
class QDialogButtonBox;
class SaveFilesDlg : public QDialog
{
    Q_OBJECT

public:
    enum Result
    {
        ResultSave = 1,
        ResultDiscard = 2,
    };
    explicit SaveFilesDlg(const QList<FileInfo>& files, QWidget *parent = nullptr);
    ~SaveFilesDlg();

    QList<int> selectedIds() const;
    int saveModeResult() const;

private slots:
    //void onSaveClicked();
    //void onSaveAllClicked();
    void onNoClicked();
    void onSaveClicked();
    void onSelectionChanged();

private:
    void setupUi();
    void updateButtonVisibility();

    Ui::SaveFilesDlg *ui;

    QStandardItemModel* m_model;
    QPushButton* m_saveButton;
    QPushButton* m_saveAllButton;
    QPushButton* m_noButton;
    QPushButton* m_noAllButton;

    int m_SaveModeResult;
    bool m_allSelected;
    QList<FileInfo> m_files;
};

#endif // SAVEFILESDLG_H
