#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QLabel>
#include <QProgressBar>
#include <QThread>
#include "ContactReader.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QThread thread;
    QLabel *statLabel;
    QLabel *progressLabel;
    QProgressBar *statProgress;
    ContactReader contactReader;
    QTreeWidgetItem *getRootItem(const QString &str);
    QTreeWidgetItem *addChildItem(QTreeWidgetItem *parentItem, const QString &str);

private slots:
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void clearContacts();
    void addContact(const Contact *contact);
    void updateProgress(int total, int pos, const QString &str);

signals:
    void open(const QString &filename);
    void save(const QString &filename);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
