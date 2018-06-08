#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <libraw.h>
#include <qfiledialog.h>
#include <qdebug.h>
#include <qerrormessage.h>
#include <qmessagebox.h>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <vector>
#include <bitset>
#include <QListWidgetItem>

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

    void on_listWidget_itemPressed(QListWidgetItem *item);

private:
    bool loadFolder(const QDir& dir);
    int loadFile(const QString& path);
    bool processRaw(const QString& path); //dodac flagi
    void rescale(const double ratio);


    /////////////////mouse handling
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
    QPoint lastPoint;
    bool pressedonImage = false;
    bool pressedonGallery = false;
    ////////////////////


    Ui::MainWindow *ui;
    QString versionNumber = "v0.1d";

    int currentImageIndex = 0;
    QImage currentImg;
    std::vector<QImage> loadedImages;
    std::bitset<128> loadedBools;
};

#endif // MAINWINDOW_H
