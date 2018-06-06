#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOtw_rz_triggered()
{
    QFileDialog dialog(this, tr("Open File"));
    dialog.setFileMode(QFileDialog::Directory);
    while (dialog.exec() == QDialog::Accepted && loadFile(dialog.directory()));
}

void MainWindow::on_actionZapisz_triggered()
{

}

void MainWindow::on_actionZapisz_jako_triggered()
{

}

void MainWindow::on_actionZamknij_triggered()
{
    this->close();
}

bool MainWindow::loadFile(const QDir& dir)
{
    bool RAWFound = true;
    qDebug() << dir.entryList();

    if (dir.isEmpty())
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Blad","Folder jest pusty !");
        messageBox.setFixedSize(500,200);
        return false;
    }

    for (QString& str : dir.entryList())
    {
        if (str.size() > 4)
        {
            qDebug() << str;
            // TODO: implement opening RAW files
        }
    }

    return false;
}
