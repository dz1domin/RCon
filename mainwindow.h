#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "C:\Users\MSI\Desktop\RCon\LibRaw-0.18.11\libraw\libraw.h"
#include <qfiledialog.h>
#include <qdebug.h>
#include <qerrormessage.h>
#include <qmessagebox.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOtw_rz_triggered();

    void on_actionZapisz_triggered();

    void on_actionZapisz_jako_triggered();

    void on_actionZamknij_triggered();

private:
    bool loadFile(const QDir& dir);

    Ui::MainWindow *ui;

    QString versionNumber = "v0.1d";
};

#endif // MAINWINDOW_H
