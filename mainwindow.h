#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QLabel>
#include <QSql>
#include <QDebug>
#include <QTableView>
#include <QStandardItem>
#include <QTreeWidgetItem>
#include <QDate>
#include <QStandardItemModel>
#include <QStringList>
#include <QTreeView>
#include <QSqlError>
#include <QDateTimeEdit>
#include <QLineEdit>
#include <QSqlRecord>

#include "depart.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool initMarksByDate(QDate date);
    bool testInitStat(QDate date);

private slots:
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_actionCon_triggered();

    void on_actionInit_triggered();

    void dateTimeEdit_init_dateChanged(QDate date);

    void on_tableView_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    bool connDb();

    QLineEdit *lineEditUsername;
    QLineEdit *lineEditPassword;
    QLineEdit *lineEditServerIp;
    QLabel *dateTimeLabel;

    QDateTimeEdit *dateTimeEdit_init;
    QSqlDatabase connDbHandle;
    QSqlTableModel *queryModel;
    QSqlTableModel *markModel;
    QSqlTableModel *departModel;

    void setTable(QString tableName,
                  QSqlTableModel *&queryModel,
                  QTableView *&tableView,
                  QSqlTableModel::EditStrategy editStrategy,
                  int hideStart,
                  int hideEnd);

    void makeDepartTree();

};

#endif // MAINWINDOW_H
