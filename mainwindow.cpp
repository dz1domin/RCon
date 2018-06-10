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
    if(ui->profileTabWidget->currentWidget() == ui->profil1Tab)
        processor.dcraw_ppm_tiff_writer("img_profile1.tiff");

    if(ui->profileTabWidget->currentWidget() == ui->profil2Tab)
        processor.dcraw_ppm_tiff_writer("img_profile2.tiff");

    if(ui->profileTabWidget->currentWidget() == ui->profil3Tab)
        processor.dcraw_ppm_tiff_writer("img_profile3.tiff");
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
    processor.recycle();

    int returnCode;
    returnCode = processor.open_file(path.toStdString().c_str());
    if(returnCode)
        return returnCode;

    int thumbCode;
    thumbCode = processor.unpack_thumb();
    QImage icon;
    if(thumbCode){
        icon.load("brak.jpeg");
        icon = icon.scaled(150,100);
        ui->listWidget->addItem(new QListWidgetItem(QIcon(QPixmap::fromImage(icon)),path));
        return false;
    }

     processor.dcraw_thumb_writer("thumb.jpeg");
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
                if(e->angleDelta().y() > 0)
                    rescale(1.25);
                else{

                    if(ui->profileTabWidget->currentWidget() == ui->profil1Tab) currentImg = imgtab1;
                    if(ui->profileTabWidget->currentWidget() == ui->profil1Tab) currentImg = imgtab2;
                    if(ui->profileTabWidget->currentWidget() == ui->profil1Tab) currentImg = imgtab3;
                    ui->ImageContainingLabel->setPixmap(QPixmap::fromImage(currentImg).scaled(currW,currH));
                    ui->ImageDisplayArea->horizontalScrollBar()->setValue(ui->ImageDisplayArea->horizontalScrollBar()->minimum());
                    ui->ImageDisplayArea->verticalScrollBar()->setValue(ui->ImageDisplayArea->verticalScrollBar()->minimum());


                }
            }
        return true;
    }
    return false;
}

void MainWindow::on_listWidget_itemPressed(QListWidgetItem *item)
{

    //usunac ten event
   // processRaw(item->text());

}

bool MainWindow::processRaw(const QString& path) //dodac flagi
{
    //processing z flagmami
    processor.recycle();
    ui->ImageDisplayArea->horizontalScrollBar()->setValue(ui->ImageDisplayArea->horizontalScrollBar()->minimum());
    ui->ImageDisplayArea->verticalScrollBar()->setValue(ui->ImageDisplayArea->verticalScrollBar()->minimum());


    int returnCode;
    returnCode = processor.open_file(path.toStdString().c_str());
    if(returnCode)
        return false;


    processor.unpack();


    currW = processor.imgdata.sizes.width;
    currH = processor.imgdata.sizes.height;
    processor.raw2image();
    setParams();

    processor.dcraw_process();

    currentImg = draw();
    if(ui->profileTabWidget->currentWidget() == ui->profil1Tab) imgtab1 = currentImg;
    if(ui->profileTabWidget->currentWidget() == ui->profil2Tab) imgtab2 = currentImg;
    if(ui->profileTabWidget->currentWidget() == ui->profil3Tab) imgtab3 = currentImg;



    ui->ImageContainingLabel->setPixmap(QPixmap::fromImage(currentImg));
    return true;
}

void MainWindow::rescale(const double ratio)
{

    double xs = static_cast<double>(ui->ImageDisplayArea->horizontalScrollBar()->value())/(ui->ImageDisplayArea->horizontalScrollBar()->maximum() - ui->ImageDisplayArea->horizontalScrollBar()->minimum());
    double ys = static_cast<double>(ui->ImageDisplayArea->verticalScrollBar()->value())/(ui->ImageDisplayArea->verticalScrollBar()->maximum() - ui->ImageDisplayArea->verticalScrollBar()->minimum());

    int xw = currentImg.width() / ratio ;
    int yh = currentImg.height() / ratio ;

    QImage temp_img = QImage(xw,yh,QImage::Format_ARGB32);

    int x_start = xs * xw;
    if(x_start + xw >= currentImg.width()){
        x_start -= (x_start+xw - currentImg.width());
    }
    int y_start = ys * yh;
    if(y_start + yh >= currentImg.height()){
        y_start -= (y_start + yh - currentImg.height());
    }


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
    ui->ImageContainingLabel->setPixmap(QPixmap::fromImage(currentImg).scaled(currW,currH));

}

void MainWindow::setParams()
{
    if(ui->profileTabWidget->currentWidget() == ui->profil1Tab){

        processor.imgdata.params.use_camera_wb = ui->P1CameraWhiteBalanceCheckBox->isChecked();
        processor.imgdata.params.use_auto_wb = ui->P1AvgImageWhiteBalanceCheckBox->isChecked();

        if(ui->P1RectAvgBalanceH->toPlainText()[0].isDigit()
                && ui->P1RectAvgBalanceW->toPlainText()[0].isDigit()
                && ui->P1RectAvgBalanceX->toPlainText()[0].isDigit()
                && ui->P1RectAvgBalanceY->toPlainText()[0].isDigit())
        {
            processor.imgdata.params.greybox[0] = ui->P1RectAvgBalanceX->toPlainText().toUInt();
            processor.imgdata.params.greybox[1] = ui->P1RectAvgBalanceY->toPlainText().toUInt();
            processor.imgdata.params.greybox[2] = ui->P1RectAvgBalanceW->toPlainText().toUInt();
            processor.imgdata.params.greybox[3] = ui->P1RectAvgBalanceH->toPlainText().toUInt();

        }
        else{
            processor.imgdata.params.greybox[0] = 0;
            processor.imgdata.params.greybox[1] = 0;
            processor.imgdata.params.greybox[2] = 0;
            processor.imgdata.params.greybox[3] = 0;
        }

        if(ui->P1CustomWhiteB->toPlainText()[0].isDigit()
                && ui->P1CustomWhiteG->toPlainText()[0].isDigit()
                && ui->P1CustomWhiteR->toPlainText()[0].isDigit()
                && ui->P1CustomWhiteG2->toPlainText()[0].isDigit())

        {
            processor.imgdata.params.user_mul[0] = ui->P1CustomWhiteR->toPlainText().toFloat();
            processor.imgdata.params.user_mul[1] = ui->P1CustomWhiteG->toPlainText().toFloat();
            processor.imgdata.params.user_mul[2] = ui->P1CustomWhiteB->toPlainText().toFloat();
            processor.imgdata.params.user_mul[3] = ui->P1CustomWhiteG2->toPlainText().toFloat();

        }
        else{
            processor.imgdata.params.user_mul[0] = 0;
            processor.imgdata.params.user_mul[1] = 0;
            processor.imgdata.params.user_mul[2] = 0;
            processor.imgdata.params.user_mul[3] = 0;
        }

        processor.imgdata.params.use_camera_matrix = ui->P1UseEmbededColorMatrixCheckBox->isChecked();
        processor.imgdata.params.output_color = ui->P1OutPutColorSpace->currentIndex();
        processor.imgdata.params.no_interpolation = ui->P1_doption->isChecked();
        processor.imgdata.params.no_auto_scale = ui->P1_Doption->isChecked();
        processor.imgdata.params.no_interpolation = ui->P1_Doption->isChecked();
        processor.imgdata.params.half_size = ui->P1_hoption->isChecked();
        processor.imgdata.params.user_qual = ui->P1InterpolationOptions->currentIndex();
        processor.imgdata.params.four_color_rgb = ui->P1_foption->isChecked();

    }
    else if(ui->profileTabWidget->currentWidget() == ui->profil2Tab)
    {
        processor.imgdata.params.use_camera_wb = ui->P2CameraWhiteBalanceCheckBox->isChecked();
        processor.imgdata.params.use_auto_wb = ui->P2AvgImageWhiteBalanceCheckBox->isChecked();

        if(ui->P2RectAvgBalanceH->toPlainText()[0].isDigit()
                && ui->P2RectAvgBalanceW->toPlainText()[0].isDigit()
                && ui->P2RectAvgBalanceX->toPlainText()[0].isDigit()
                && ui->P2RectAvgBalanceY->toPlainText()[0].isDigit())
        {
            processor.imgdata.params.greybox[0] = ui->P2RectAvgBalanceX->toPlainText().toUInt();
            processor.imgdata.params.greybox[1] = ui->P2RectAvgBalanceY->toPlainText().toUInt();
            processor.imgdata.params.greybox[2] = ui->P2RectAvgBalanceW->toPlainText().toUInt();
            processor.imgdata.params.greybox[3] = ui->P2RectAvgBalanceH->toPlainText().toUInt();

        }
        else{
            processor.imgdata.params.greybox[0] = 0;
            processor.imgdata.params.greybox[1] = 0;
            processor.imgdata.params.greybox[2] = 0;
            processor.imgdata.params.greybox[3] = 0;
        }

        if(ui->P2CustomWhiteB->toPlainText()[0].isDigit()
                && ui->P2CustomWhiteG->toPlainText()[0].isDigit()
                && ui->P2CustomWhiteR->toPlainText()[0].isDigit()
                && ui->P2CustomWhiteG2->toPlainText()[0].isDigit())

        {
            processor.imgdata.params.user_mul[0] = ui->P2CustomWhiteR->toPlainText().toFloat();
            processor.imgdata.params.user_mul[1] = ui->P2CustomWhiteG->toPlainText().toFloat();
            processor.imgdata.params.user_mul[2] = ui->P2CustomWhiteB->toPlainText().toFloat();
            processor.imgdata.params.user_mul[3] = ui->P2CustomWhiteG2->toPlainText().toFloat();

        }
        else{
            processor.imgdata.params.user_mul[0] = 0;
            processor.imgdata.params.user_mul[1] = 0;
            processor.imgdata.params.user_mul[2] = 0;
            processor.imgdata.params.user_mul[3] = 0;
        }

        processor.imgdata.params.use_camera_matrix = ui->P2UseEmbededColorMatrixCheckBox->isChecked();
        processor.imgdata.params.output_color = ui->P2OutPutColorSpace->currentIndex();
        processor.imgdata.params.no_interpolation = ui->P2_doption->isChecked();
        processor.imgdata.params.no_auto_scale = ui->P2_Doption->isChecked();
        processor.imgdata.params.no_interpolation = ui->P2_Doption->isChecked();
        processor.imgdata.params.half_size = ui->P2_hoption->isChecked();
        processor.imgdata.params.user_qual = ui->P2InterpolationOptions->currentIndex();
        processor.imgdata.params.four_color_rgb = ui->P2_foption->isChecked();
    }
    else if(ui->profileTabWidget->currentWidget() == ui->profil3Tab)
    {
        processor.imgdata.params.use_camera_wb = ui->P3CameraWhiteBalanceCheckBox->isChecked();
        processor.imgdata.params.use_auto_wb = ui->P3AvgImageWhiteBalanceCheckBox->isChecked();

        if(ui->P3RectAvgBalanceH->toPlainText()[0].isDigit()
                && ui->P3RectAvgBalanceW->toPlainText()[0].isDigit()
                && ui->P3RectAvgBalanceX->toPlainText()[0].isDigit()
                && ui->P3RectAvgBalanceY->toPlainText()[0].isDigit())
        {
            processor.imgdata.params.greybox[0] = ui->P3RectAvgBalanceX->toPlainText().toUInt();
            processor.imgdata.params.greybox[1] = ui->P3RectAvgBalanceY->toPlainText().toUInt();
            processor.imgdata.params.greybox[2] = ui->P3RectAvgBalanceW->toPlainText().toUInt();
            processor.imgdata.params.greybox[3] = ui->P3RectAvgBalanceH->toPlainText().toUInt();

        }
        else{
            processor.imgdata.params.greybox[0] = 0;
            processor.imgdata.params.greybox[1] = 0;
            processor.imgdata.params.greybox[2] = 0;
            processor.imgdata.params.greybox[3] = 0;
        }

        if(ui->P3CustomWhiteB->toPlainText()[0].isDigit()
                && ui->P3CustomWhiteG->toPlainText()[0].isDigit()
                && ui->P3CustomWhiteR->toPlainText()[0].isDigit()
                && ui->P3CustomWhiteG2->toPlainText()[0].isDigit())

        {
            processor.imgdata.params.user_mul[0] = ui->P3CustomWhiteR->toPlainText().toFloat();
            processor.imgdata.params.user_mul[1] = ui->P3CustomWhiteG->toPlainText().toFloat();
            processor.imgdata.params.user_mul[2] = ui->P3CustomWhiteB->toPlainText().toFloat();
            processor.imgdata.params.user_mul[3] = ui->P3CustomWhiteG2->toPlainText().toFloat();

        }
        else{
            processor.imgdata.params.user_mul[0] = 0;
            processor.imgdata.params.user_mul[1] = 0;
            processor.imgdata.params.user_mul[2] = 0;
            processor.imgdata.params.user_mul[3] = 0;
        }

        processor.imgdata.params.use_camera_matrix = ui->P3UseEmbededColorMatrixCheckBox->isChecked();
        processor.imgdata.params.output_color = ui->P3OutPutColorSpace->currentIndex();
        processor.imgdata.params.no_interpolation = ui->P3_doption->isChecked();
        processor.imgdata.params.no_auto_scale = ui->P3_Doption->isChecked();
        processor.imgdata.params.no_interpolation = ui->P3_Doption->isChecked();
        processor.imgdata.params.half_size = ui->P3_hoption->isChecked();
        processor.imgdata.params.user_qual = ui->P3InterpolationOptions->currentIndex();
        processor.imgdata.params.four_color_rgb = ui->P3_foption->isChecked();
    }

}


QImage MainWindow::draw(bool inColor)
{
    int h = processor.imgdata.sizes.height;
    int w = processor.imgdata.sizes.width;
    QImage img(w,h,QImage::Format_ARGB32);
    img.fill(Qt::black);
    if(inColor){
        libraw_processed_image_t *todelete = processor.dcraw_make_mem_image();
        uchar* img_raw = todelete->data;

        for(int y=0; y<h;++y){
            uchar *line = img.scanLine(y);
            for(int x=0; x<w*4;x+=4){
                line[x] = img_raw[(y * w + x/4)*3 + 2]; //b
                line[x+1] = img_raw[(y * w + x/4)*3 + 1]; //g
                line[x+2] = img_raw[(y * w + x/4)*3]; //r
                line[x+3] =255; //alpha
            }
        }
        processor.dcraw_clear_mem(todelete);
    }
    else{
        //TODO MONOCOLOR
    }


    return img;
}


void MainWindow::on_actionPrzetworz_wybrane_zdjecie_triggered()
{
    processRaw(ui->listWidget->currentItem()->text());
    ui->listWidget->clearFocus();
    ui->listWidget->clearSelection();
}

void MainWindow::on_profileTabWidget_currentChanged(int index)
{
    ui->ImageDisplayArea->horizontalScrollBar()->setValue(ui->ImageDisplayArea->horizontalScrollBar()->minimum());
    ui->ImageDisplayArea->verticalScrollBar()->setValue(ui->ImageDisplayArea->verticalScrollBar()->minimum());

    switch(index){
    case 0:
        currentImg = imgtab1;
        break;
    case 1:
        currentImg = imgtab2;
        break;
    case 2:
        currentImg = imgtab3;
        break;
    }
    ui->ImageContainingLabel->setPixmap(QPixmap::fromImage(currentImg));
}
