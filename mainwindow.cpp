#include <QDebug>
#include <QFileDialog>
#include <QStandardPaths>
#include "mainwindow.h"
#include "ui_mainwindow.h"
//--------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , statLabel(NULL)
    , statProgress(NULL)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    statLabel = new QLabel(this);
    progressLabel = new QLabel(this);
    statProgress = new QProgressBar(this);
    statProgress->setTextVisible(false);
    statProgress->setFormat("%v/%m");
    ui->statusBar->addPermanentWidget(statLabel);
    ui->statusBar->addPermanentWidget(progressLabel);
    ui->statusBar->addPermanentWidget(statProgress);

    contactReader.moveToThread(&thread);
    thread.start();

    connect(&contactReader, SIGNAL(clearContacts()), this, SLOT(clearContacts()));
    connect(&contactReader, SIGNAL(addContact(const Contact*)), this, SLOT(addContact(const Contact*)));
    connect(&contactReader, SIGNAL(updateProgress(int,int,QString)), this, SLOT(updateProgress(int,int,QString)));
    connect(this, SIGNAL(open(QString)), &contactReader, SLOT(open(QString)));
    connect(this, SIGNAL(save(QString)), &contactReader, SLOT(save(QString)));
}
//--------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    thread.terminate();
    thread.wait(500);

    delete ui;
}
//--------------------------------------------------------------------------------
void MainWindow::on_actionOpen_triggered()
{
    ui->actionOpen->setEnabled(false);
    ui->actionSave->setEnabled(false);

    QString location = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    qDebug() << "location" << location;

    QString filename = QFileDialog::getOpenFileName(this, tr("select mail2000 contact file"), location, tr("Mail2000 Contact (*.csv)"));
    qDebug() << "filename" << filename;

    emit open(filename);
}
//--------------------------------------------------------------------------------
void MainWindow::clearContacts()
{
    ui->treeWidget->clear();
    statProgress->setMaximum(1);
    statProgress->setValue(0);
    statLabel->setText("");
    progressLabel->setText("");
}
//--------------------------------------------------------------------------------
void MainWindow::addContact(const Contact *contact)
{
//    qDebug() << "department:" << contact->department << "|"
//             << "nickName:" << contact->nickName << "|"
//             << "email:" << contact->email << "|"
//             << "firstName:" << contact->firstName << "|"
//             << "lastName:" << contact->lastName << "|"
//             << "cellPhone:" << contact->cellPhone << "|"
//             << "extNumber:" << contact->extNumber << "|"
//             << "remark:" << contact->remark;

    QTreeWidgetItem *departmentItem = getRootItem(contact->department);
    QTreeWidgetItem *nameItem = addChildItem(departmentItem, contact->lastName + contact->firstName + "(" + contact->nickName + ")");

    if(!contact->email.isEmpty())
        addChildItem(nameItem, QString("EMAIL: ") + contact->email);

    if(!contact->cellPhone.isEmpty())
        addChildItem(nameItem, QString("手機: ") + contact->cellPhone);

    if(!contact->extNumber.isEmpty())
        addChildItem(nameItem, QString("分機: ") + contact->extNumber);
}
//--------------------------------------------------------------------------------
void MainWindow::on_actionSave_triggered()
{
    ui->actionOpen->setEnabled(false);
    ui->actionSave->setEnabled(false);

    QString location = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    qDebug() << "location" << location;

    QString filename = QFileDialog::getSaveFileName(this, tr("save as vcard"), location, tr("VCard (*.vcf)"));
    qDebug() << "filename" << filename;

    emit save(filename);
}
//--------------------------------------------------------------------------------
QTreeWidgetItem * MainWindow::getRootItem(const QString &str)
{
    QTreeWidgetItem *rootItem = NULL;

    for(int i = ui->treeWidget->topLevelItemCount() - 1; i >= 0; --i)
    {
        rootItem = ui->treeWidget->topLevelItem(i);
        if(rootItem->text(0) == str)
            return rootItem;
    }

    rootItem = new QTreeWidgetItem();
    rootItem->setText(0, str);
    ui->treeWidget->addTopLevelItem(rootItem);

    return rootItem;
}
//--------------------------------------------------------------------------------
QTreeWidgetItem * MainWindow::addChildItem(QTreeWidgetItem *parentItem, const QString &str)
{
    QTreeWidgetItem *childItem = new QTreeWidgetItem();
    childItem->setText(0, str);
    parentItem->addChild(childItem);
    return childItem;
}
//--------------------------------------------------------------------------------
void MainWindow::updateProgress(int total, int pos, const QString &str)
{
    if(statProgress->maximum() != total)
        statProgress->setMaximum(total);
    statProgress->setValue(pos);
    statLabel->setText(str);
    progressLabel->setText(statProgress->text());

    if(total == pos)
    {
        ui->actionOpen->setEnabled(true);
        ui->actionSave->setEnabled(true);
    }
}
//--------------------------------------------------------------------------------
