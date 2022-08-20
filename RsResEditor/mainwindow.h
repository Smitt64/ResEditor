#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "rsrescore_global.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class ResLib;
class QMdiArea;
class ResListDockWidget;
class PropertyDockWidget;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void doubleResClicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;

    ResListDockWidget *m_ResListDoc;
    PropertyDockWidget *m_PropertyDoc;
    QMdiArea *m_Mdi;

    ResLib *m_Lib;
};
#endif // MAINWINDOW_H
