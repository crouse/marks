#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    lineEditUsername = new QLineEdit();
    lineEditUsername->setFixedWidth(100);
    lineEditUsername->setPlaceholderText("用户名");
    lineEditPassword = new QLineEdit();
    lineEditPassword->setFixedWidth(100);
    lineEditPassword->setPlaceholderText("密码");
    lineEditServerIp = new QLineEdit();
    lineEditServerIp->setPlaceholderText("服务器地址");
    lineEditServerIp->setFixedWidth(100);

    dateTimeEdit_init = new QDateTimeEdit();
    dateTimeEdit_init->setAlignment(Qt::AlignCenter);
    dateTimeEdit_init->setDisplayFormat("yyyy-MM-dd");
    dateTimeEdit_init->setDate(QDate::currentDate());
    dateTimeEdit_init->setCalendarPopup(true);

    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addWidget(lineEditUsername);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addWidget(lineEditPassword);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addWidget(lineEditServerIp);
    ui->mainToolBar->addSeparator();
    dateTimeLabel = new QLabel("📅");
    ui->mainToolBar->addWidget(dateTimeLabel);
    ui->mainToolBar->addWidget(dateTimeEdit_init);

    connect(dateTimeEdit_init, SIGNAL(dateChanged(QDate)), this, SLOT(dateTimeEdit_init_dateChanged(QDate)));
}

MainWindow::~MainWindow()
{
    delete ui;
}


bool MainWindow::connDb()
{
    connDbHandle = QSqlDatabase::addDatabase("QMYSQL");
    connDbHandle.setDatabaseName("vol");
    connDbHandle.setHostName("127.0.0.1");
    connDbHandle.setUserName("root");
    connDbHandle.setPassword("tbontBtiaQ@!");

    if(!connDbHandle.open()) return false;

    setTable("volunteer", queryModel, ui->tableView, QSqlTableModel::OnFieldChange, 4, 35);
    setTable("marks", markModel, ui->tableViewMark, QSqlTableModel::OnFieldChange, 1, 1);

    return true;
}

void MainWindow::setTable(QString tableName,
                          QSqlTableModel *&queryModel,
                          QTableView *&tableView,
                          QSqlTableModel::EditStrategy editStrategy,
                          int hideStart,
                          int hideEnd)
{
    QSqlQuery query;
    QString sql = QString("show full columns from `%1`").arg(tableName);
    query.exec(sql);

    queryModel = new QSqlTableModel(this);
    queryModel->setTable(tableName);
    queryModel->setEditStrategy(editStrategy);
    queryModel->setSort(0, Qt::AscendingOrder);

    int i = 0;
    while(query.next()) {
        QString comment = query.value(8).toString();
        queryModel->setHeaderData(i, Qt::Horizontal, comment);
        i++;
    }

    queryModel->select();

    tableView->setModel(queryModel);
    tableView->setColumnHidden(0, true);
    for(int i = hideStart; i <= hideEnd; i++) {
        tableView->setColumnHidden(i, true);
    }

    tableView->horizontalHeader()->setStretchLastSection(true);
    tableView->reset();
}

void MainWindow::makeDepartTree()
{
    int level_max = 0;
    ui->treeWidget->setColumnCount(1);
    ui->treeWidget->setHeaderLabel("|--部门选择树--|");

    QList <Depart*> list;
    QSqlQuery query;
    query.exec("select id, parent_id, dname, level from tb_depart");
    while(query.next()) {
        Depart *d = new Depart();
        d->id = query.value(0).toInt();
        d->parentId = query.value(1).toInt();
        d->dname = query.value(2).toString();
        d->level = query.value(3).toInt();
        list.append(d);
    }

    Depart *dep;
    QTreeWidgetItem *item[100];

    foreach(dep, list) {
        if (dep->level > level_max) level_max = dep->level;
        if (dep->level == 0) item[dep->id] = new QTreeWidgetItem(ui->treeWidget, QStringList(dep->dname));
    }

    qDebug() << "level_max:" << level_max;

    for(int i = 1; i <= level_max; i++) {
        foreach(dep, list) {
            if (dep->level == i) {
                item[dep->id] = new QTreeWidgetItem(item[dep->parentId], QStringList(dep->dname));
                item[dep->parentId]->addChild(item[dep->id]);
            }
        }
    }

    ui->treeWidget->expandAll();
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    QString filter;
    QString qfilter;
    QString depart;
    QString departsecond;
    QTreeWidgetItem *parent = item->parent();

    depart = item->data(0, 0).toString();

    if (NULL == parent) {
        filter = QString(" cdate = '%2' and pid in (select pid from volunteer where depart = '%1') ").arg(depart).arg(dateTimeEdit_init->date().toString("yyyy-MM-dd"));
        qfilter = QString(" depart = '%1' ").arg(depart);
    } else {
        departsecond = depart;
        depart = parent->data(0, 0).toString();
        filter = QString(" cdate = '%3' and pid in (select pid from volunteer where depart = '%1' and departsecond = '%2') ")
                .arg(depart).arg(departsecond).arg(dateTimeEdit_init->date().toString("yyyy-MM-dd"));
        qfilter = QString(" depart = '%1' and departsecond = '%2' ").arg(depart).arg(departsecond);
    }

    markModel->setFilter(filter);
    markModel->select();
    ui->tableViewMark->reset();
    qDebug() << filter << column;

    queryModel->setFilter(qfilter);
    queryModel->select();
    ui->tableView->reset();

}

void MainWindow::on_actionCon_triggered()
{
    if (connDb()) {
        makeDepartTree();
        ui->actionCon->setDisabled(true);
    }
}
void MainWindow::on_actionInit_triggered()
{
    initMarksByDate(dateTimeEdit_init->date());
}

bool MainWindow::initMarksByDate(QDate date)
{
    if (testInitStat(date)) {
        markModel->setFilter(QString(" cdate = '%1' ").arg(date.toString("yyyy-MM-dd")));
        markModel->select();
        ui->tableViewMark->reset();
        return false;
    }

    QString cdate = date.toString("yyyy-MM-dd");
    QSqlQuery query, queryS;
    query.exec("select name, pid from volunteer");
    while(query.next()) {
        QString name = query.value(0).toString();
        QString pid = query.value(1).toString();
        qDebug() << name << pid << cdate;
        queryS.prepare("insert into marks(pid, name, cdate) values (:pid, :name, :cdate)");
        queryS.bindValue(":pid", pid);
        queryS.bindValue(":name", name);
        queryS.bindValue(":cdate", cdate);
        queryS.exec();
        qDebug() << queryS.lastError();
    }

    query.prepare("insert into init_record(cdate, stat) values (:cdate, 1)");
    query.bindValue(":cdate", cdate);
    query.exec();

    markModel->setFilter(QString(" cdate = '%1' ").arg(date.toString("yyyy-MM-dd")));
    markModel->select();
    ui->tableViewMark->reset();

    return true;
}

bool MainWindow::testInitStat(QDate date)
{
    QSqlQuery query;
    query.prepare("select stat from init_record where cdate = :cdate");
    query.bindValue(":cdate", date.toString("yyyy-MM-dd"));
    query.exec();
    if (query.next()) return true;
    return false;
}

void MainWindow::dateTimeEdit_init_dateChanged(QDate date)
{
    qDebug() << "on_dateTimeEdit_init_changed triggered" << date;
    initMarksByDate(date);
}

void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
    int col = queryModel->record().indexOf("pid");
    int markCol = markModel->record().indexOf("cdate");
    QString pid = queryModel->record(index.row()).value(col).toString();
    markModel->setFilter(QString(" pid = (select pid from volunteer where pid = '%1') ").arg(pid));
    markModel->setSort(markCol, Qt::AscendingOrder);
    markModel->select();
    ui->tableViewMark->reset();
}

