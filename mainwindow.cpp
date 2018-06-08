#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QCoreApplication::instance()->installEventFilter(this);
    ui->listWidget->setViewMode(QListWidget::IconMode);
    ui->listWidget->setIconSize(QSize(150,100));
    ui->listWidget->setResizeMode(QListWidget::Adjust);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOtw_rz_triggered()
{
    QFileDialog dialog(this, tr("Open File"));
    dialog.setFileMode(QFileDialog::Directory);
    while (dialog.exec() == QDialog::Accepted && loadFolder(dialog.directory()));
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

bool MainWindow::loadFolder(const QDir& dir)
{
    bool RAWFound = false;
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

            switch(loadFile(dir.path() + "/"+ str))
            {
                case 0:
                    RAWFound = true;
                    break;
                 //TODO uzupelnic bledy z dokumentacji
                 //tzn messageboxy
            }


        }
    }

    return false;
}

int MainWindow::loadFile(const QString& path)
{
    LibRaw lib;
    int returnCode;
    returnCode = lib.open_file(path.toStdString().c_str());
    if(returnCode)
        return returnCode;

    int thumbCode;
    thumbCode = lib.unpack_thumb();
    QImage icon;
    if(thumbCode){
        icon.load("brak.jpeg");
        icon = icon.scaled(150,100);
        ui->listWidget->addItem(new QListWidgetItem(QIcon(QPixmap::fromImage(icon)),path));
        return false;
    }

     lib.dcraw_thumb_writer("thumb.jpeg");
     icon.load("thumb.jpeg");
     icon = icon.scaled(150,100);
     ui->listWidget->addItem(new QListWidgetItem(QIcon(QPixmap::fromImage(icon)),path));
     return true;
}


void MainWindow::mousePressEvent(QMouseEvent *e)
{


    if(e->pos().x() > ui->ImageContainingLabel->x() && e->pos().x() < ui->ImageContainingLabel->x() + ui->ImageContainingLabel->width()
            && e->pos().x() > ui->ImageContainingLabel->y() && e->pos().y() < ui->ImageContainingLabel->y() + ui->ImageContainingLabel->height())
    {
        QApplication::setOverrideCursor(Qt::ClosedHandCursor);
        pressedonImage = true;
    }

    lastPoint = e->pos();

}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    pressedonGallery = false;
    pressedonImage = false;
    QApplication::setOverrideCursor(Qt::ArrowCursor);

}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(pressedonImage){
        QPoint div = lastPoint - event->pos();
        ui->ImageDisplayArea->horizontalScrollBar()->setValue(ui->ImageDisplayArea->horizontalScrollBar()->value() + div.x());
        ui->ImageDisplayArea->verticalScrollBar()->setValue(ui->ImageDisplayArea->verticalScrollBar()->value() + div.y());
    }
    lastPoint = event->pos();
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *e)
{
    if(e->pos().x() > ui->ImageContainingLabel->x() && e->pos().x() < ui->ImageContainingLabel->x() + ui->ImageContainingLabel->width()
            && e->pos().x() > ui->ImageContainingLabel->y() && e->pos().y() < ui->ImageContainingLabel->y() + ui->ImageContainingLabel->height())
    {
        rescale(2.0);
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::Wheel){
        QWheelEvent *e = static_cast<QWheelEvent*>(event);
        if(e->pos().x() > ui->ImageContainingLabel->x() && e->pos().x() < ui->ImageContainingLabel->x() + ui->ImageContainingLabel->width()
                    && e->pos().x() > ui->ImageContainingLabel->y() && e->pos().y() < ui->ImageContainingLabel->y() + ui->ImageContainingLabel->height())
            {
                rescale(1.25);
            }
        return true;
    }
    return false;
}

void MainWindow::on_listWidget_itemPressed(QListWidgetItem *item)
{
    qDebug() << item->text();
    if(!loadedBools[ui->listWidget->currentRow()])
        processRaw(item->text());
    else{
        currentImg = loadedImages[ui->listWidget->currentRow()];
        currentImageIndex = ui->listWidget->currentRow();
        ui->ImageContainingLabel->setPixmap(QPixmap::fromImage(currentImg));
    }


}

bool MainWindow::processRaw(const QString& path) //dodac flagi
{
    //processing z flagmami

    LibRaw lib;
    int returnCode;
    returnCode = lib.open_file(path.toStdString().c_str());
    if(returnCode)
        return false;

    lib.unpack();
    lib.raw2image();
    lib.dcraw_process();
    uchar* img_raw = lib.dcraw_make_mem_image()->data;
    int h = lib.imgdata.sizes.height;
    int w = lib.imgdata.sizes.width;

    QImage img(w,h,QImage::Format_ARGB32);
    img.fill(Qt::black);


    for(int y=0; y<h;++y){
        uchar *line = img.scanLine(y);
        for(int x=0; x<w*4;x+=4){
            line[x] = img_raw[(y * w + x/4)*3 + 2]; //b
            line[x+1] = img_raw[(y * w + x/4)*3 + 1]; //g
            line[x+2] = img_raw[(y * w + x/4)*3]; //r
            line[x+3] =255; //alpha

        }
    }

    loadedImages.emplace_back(std::move(img));
    currentImageIndex = loadedImages.size() - 1;
    currentImg = loadedImages[currentImageIndex];
    ui->ImageContainingLabel->setPixmap(QPixmap::fromImage(currentImg));
    loadedBools[currentImageIndex] = true;

    return true;
}

void MainWindow::rescale(const double ratio)
{

    double xs = static_cast<double>(ui->ImageDisplayArea->horizontalScrollBar()->value())/(ui->ImageDisplayArea->horizontalScrollBar()->maximum() - ui->ImageDisplayArea->horizontalScrollBar()->minimum());
    double ys = static_cast<double>(ui->ImageDisplayArea->verticalScrollBar()->value())/(ui->ImageDisplayArea->verticalScrollBar()->maximum() - ui->ImageDisplayArea->verticalScrollBar()->minimum());

    qDebug() << xs << ys;


    int xw = currentImg.width() / ratio ;
    int yh = currentImg.height() / ratio ;

    qDebug() << xw << yh;

    QImage temp_img = QImage(xw,yh,QImage::Format_ARGB32);

    int x_start = xs * xw;
    int y_start = ys * yh;
    qDebug() << x_start << y_start;

    for(int y=y_start; y<y_start+yh;++y){
        uchar *org = currentImg.scanLine(y);
        uchar *temp = temp_img.scanLine(y-y_start);
        for(int x=x_start*4; x<(x_start+xw)*4;x+=4){
            temp[x-x_start*4] = org[x]; //b
            temp[x+1-x_start*4] = org[x+1]; //g
            temp[x+2-x_start*4] = org[x+2]; //r
            temp[x+3-x_start*4] =255; //alpha
        }
    }
    currentImg = std::move(temp_img);
    ui->ImageContainingLabel->setPixmap(QPixmap::fromImage(currentImg).scaled(loadedImages[currentImageIndex].width(),loadedImages[currentImageIndex].height()));

}
