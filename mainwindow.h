#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QLabel>
#include <QProgressBar>
#include <QThread>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
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

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent *event);
    virtual void dragLeaveEvent(QDragLeaveEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    QLabel *statLabel;
    QLabel *progressLabel;
    QProgressBar *statProgress;
    QThread thread;
    ContactReader contactReader;

private:
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
};

#endif // MAINWINDOW_H
