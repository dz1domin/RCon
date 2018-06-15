#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImageIOPlugin>
#include <QImageIOHandler>

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
        imgtab1.save("imgprofile1.jpeg");
    if(ui->profileTabWidget->currentWidget() == ui->profil2Tab)
        imgtab2.save("imgprofile2.jpeg");
    if(ui->profileTabWidget->currentWidget() == ui->profil3Tab)
        imgtab3.save("imgprofile3.jpeg");

}

void MainWindow::on_actionZapisz_jako_triggered()
{
    QFileDialog dialog(this, tr("Zapisz jako"));
    dialog.setDefaultSuffix("jpg");
    dialog.selectMimeTypeFilter("image/jpeg");
    dialog.setAcceptMode( QFileDialog::AcceptSave );

    QStringList mimeTypeFilters;
       const QByteArrayList supportedMimeTypes = QImageWriter::supportedMimeTypes();
       foreach (const QByteArray &mimeTypeName, supportedMimeTypes)
           mimeTypeFilters.append(mimeTypeName);
       mimeTypeFilters.sort();
       dialog.setMimeTypeFilters(mimeTypeFilters);

    while (dialog.exec() == QDialog::Accepted && !saveFile(dialog.selectedFiles().first())) {}
}

bool MainWindow::saveFile(const QString& path)
{

    QImageWriter writer(path);
    QMessageBox msg;
    msg.setFixedSize(500,200);

    if(ui->profileTabWidget->currentWidget() == ui->profil1Tab)
        if(!writer.write(imgtab1)){
            if(imgtab1.isNull())
                msg.critical(0,"Blad","Nie udalo sie zapisac pliku. Nie ma pliku na danym profilu");
            else
                msg.critical(0,"Blad","Nie udalo sie zapisac pliku. Nieznany blad");
            return false;
        }
    if(ui->profileTabWidget->currentWidget() == ui->profil2Tab)
        if(!writer.write(imgtab2)){
            if(imgtab2.isNull())
                msg.critical(0,"Blad","Nie udalo sie zapisac pliku. Nie ma pliku na danym profilu");
            else
                msg.critical(0,"Blad","Nie udalo sie zapisac pliku. Nieznany blad");
            return false;
        }

    if(ui->profileTabWidget->currentWidget() == ui->profil3Tab)
        if(!writer.write(imgtab3)){
            if(imgtab3.isNull())
                msg.critical(0,"Blad","Nie udalo sie zapisac pliku. Nie ma pliku na danym profilu");
            else
                msg.critical(0,"Blad","Nie udalo sie zapisac pliku. Nieznany blad");
            return false;
        }
    return true;
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
        messageBox.setFixedSize(500,200);
        messageBox.critical(0,"Blad","Folder jest pusty !");
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

    galleryLoaded = true;
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
                    if(ui->profileTabWidget->currentWidget() == ui->profil2Tab) currentImg = imgtab2;
                    if(ui->profileTabWidget->currentWidget() == ui->profil3Tab) currentImg = imgtab3;
                    ui->ImageContainingLabel->setPixmap(QPixmap::fromImage(currentImg).scaled(currW,currH));
                    ui->ImageDisplayArea->horizontalScrollBar()->setValue(ui->ImageDisplayArea->horizontalScrollBar()->minimum());
                    ui->ImageDisplayArea->verticalScrollBar()->setValue(ui->ImageDisplayArea->verticalScrollBar()->minimum());

                }
            }
        return true;
    }
    return false;
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

    if(processor.imgdata.params.no_interpolation)
        currentImg = draw(false);
    else
        currentImg = draw();


    if(ui->profileTabWidget->currentWidget() == ui->profil1Tab){
        imgtab1 = currentImg;
        luminanceHistogram1 = luminance_histogram();
        ui->P1histogramLabel->setPixmap(QPixmap::fromImage(luminanceHistogram1));
    }
    if(ui->profileTabWidget->currentWidget() == ui->profil2Tab){
        imgtab2 = currentImg;
        luminanceHistogram2 = luminance_histogram();
        ui->P2histogramLabel->setPixmap(QPixmap::fromImage(luminanceHistogram2));
    }
    if(ui->profileTabWidget->currentWidget() == ui->profil3Tab){
        imgtab3 = currentImg;
        luminanceHistogram3 = luminance_histogram();
        ui->P3histogramLabel->setPixmap(QPixmap::fromImage(luminanceHistogram3));
    }

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


        processor.imgdata.params.greybox[0] = ui->P1RectAvgBalanceX->value();
        processor.imgdata.params.greybox[1] = ui->P1RectAvgBalanceY->value();
        processor.imgdata.params.greybox[2] = ui->P1RectAvgBalanceW->value();
        processor.imgdata.params.greybox[3] = ui->P1RectAvgBalanceH->value();

        processor.imgdata.params.user_mul[0] = ui->P1CustomWhiteR->value() / 100.f;
        processor.imgdata.params.user_mul[1] = ui->P1CustomWhiteG->value() / 100.f;
        processor.imgdata.params.user_mul[2] = ui->P1CustomWhiteB->value() / 100.f;
        processor.imgdata.params.user_mul[3] = ui->P1CustomWhiteG2->value() / 100.f;

        processor.imgdata.params.use_camera_matrix = ui->P1UseEmbededColorMatrixCheckBox->isChecked();
        processor.imgdata.params.output_color = ui->P1OutPutColorSpace->currentIndex();

        processor.imgdata.params.no_interpolation = (ui->P1_doption->isChecked() || ui->P1_Doption->isChecked());
        processor.imgdata.params.no_auto_scale = ui->P1_Doption->isChecked();

        processor.imgdata.params.half_size = ui->P1_hoption->isChecked();
        processor.imgdata.params.user_qual = ui->P1InterpolationOptions->currentIndex();
        processor.imgdata.params.four_color_rgb = ui->P1_foption->isChecked();


        processor.imgdata.params.user_black = ui->P1_koption->value();
        processor.imgdata.params.threshold = ui->P1_noption->value();


        processor.imgdata.params.user_sat = ui->P1_Soption->value();
        processor.imgdata.params.adjust_maximum_thr = static_cast<float>(ui->P1_coption->value()) / 100.f;



    }
    else if(ui->profileTabWidget->currentWidget() == ui->profil2Tab)
    {
        processor.imgdata.params.use_camera_wb = ui->P2CameraWhiteBalanceCheckBox->isChecked();
        processor.imgdata.params.use_auto_wb = ui->P2AvgImageWhiteBalanceCheckBox->isChecked();


        processor.imgdata.params.greybox[0] = ui->P2RectAvgBalanceX->value();
        processor.imgdata.params.greybox[1] = ui->P2RectAvgBalanceY->value();
        processor.imgdata.params.greybox[2] = ui->P2RectAvgBalanceW->value();
        processor.imgdata.params.greybox[3] = ui->P2RectAvgBalanceH->value();

        processor.imgdata.params.user_mul[0] = ui->P2CustomWhiteR->value() / 100.f;
        processor.imgdata.params.user_mul[1] = ui->P2CustomWhiteG->value() / 100.f;
        processor.imgdata.params.user_mul[2] = ui->P2CustomWhiteB->value() / 100.f;
        processor.imgdata.params.user_mul[3] = ui->P2CustomWhiteG2->value() / 100.f;

        processor.imgdata.params.use_camera_matrix = ui->P2UseEmbededColorMatrixCheckBox->isChecked();
        processor.imgdata.params.output_color = ui->P2OutPutColorSpace->currentIndex();

        processor.imgdata.params.no_interpolation = (ui->P2_doption->isChecked() || ui->P2_Doption->isChecked());
        processor.imgdata.params.no_auto_scale = ui->P2_Doption->isChecked();

        processor.imgdata.params.half_size = ui->P2_hoption->isChecked();
        processor.imgdata.params.user_qual = ui->P2InterpolationOptions->currentIndex();
        processor.imgdata.params.four_color_rgb = ui->P2_foption->isChecked();


        processor.imgdata.params.user_black = ui->P2_koption->value();
        processor.imgdata.params.threshold = ui->P2_noption->value();

        processor.imgdata.params.user_sat = ui->P2_Soption->value();
        processor.imgdata.params.adjust_maximum_thr = static_cast<float>(ui->P2_coption->value()) / 100.f;
    }
    else if(ui->profileTabWidget->currentWidget() == ui->profil3Tab)
    {
        processor.imgdata.params.use_camera_wb = ui->P3CameraWhiteBalanceCheckBox->isChecked();
        processor.imgdata.params.use_auto_wb = ui->P3AvgImageWhiteBalanceCheckBox->isChecked();


        processor.imgdata.params.greybox[0] = ui->P3RectAvgBalanceX->value();
        processor.imgdata.params.greybox[1] = ui->P3RectAvgBalanceY->value();
        processor.imgdata.params.greybox[2] = ui->P3RectAvgBalanceW->value();
        processor.imgdata.params.greybox[3] = ui->P3RectAvgBalanceH->value();

        processor.imgdata.params.user_mul[0] = ui->P3CustomWhiteR->value() / 100.f;
        processor.imgdata.params.user_mul[1] = ui->P3CustomWhiteG->value() / 100.f;
        processor.imgdata.params.user_mul[2] = ui->P3CustomWhiteB->value() / 100.f;
        processor.imgdata.params.user_mul[3] = ui->P3CustomWhiteG2->value() / 100.f;

        processor.imgdata.params.use_camera_matrix = ui->P3UseEmbededColorMatrixCheckBox->isChecked();
        processor.imgdata.params.output_color = ui->P3OutPutColorSpace->currentIndex();

        processor.imgdata.params.no_interpolation = (ui->P3_doption->isChecked() || ui->P3_Doption->isChecked());
        processor.imgdata.params.no_auto_scale = ui->P3_Doption->isChecked();

        processor.imgdata.params.half_size = ui->P3_hoption->isChecked();
        processor.imgdata.params.user_qual = ui->P3InterpolationOptions->currentIndex();
        processor.imgdata.params.four_color_rgb = ui->P3_foption->isChecked();


        processor.imgdata.params.user_black = ui->P3_koption->value();
        processor.imgdata.params.threshold = ui->P3_noption->value();


        processor.imgdata.params.user_sat = ui->P3_Soption->value();
        processor.imgdata.params.adjust_maximum_thr = static_cast<float>(ui->P3_coption->value()) / 100.f;
    }

}


QImage MainWindow::draw(bool inColor)
{
    int h = processor.imgdata.sizes.height;
    int w = processor.imgdata.sizes.width;
    QImage img(w,h,QImage::Format_ARGB32);
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

    if(!inColor){
        for(int y=0; y<h;++y){
            uchar *line = img.scanLine(y);
            for(int x=0; x<w*4;x+=4){
                int grayscale = 0.2126 * line[x+2] + 0.7152 * line[x+1] + 0.0722* line[x];
                line[x] = grayscale; //b
                line[x+1] = grayscale; //g
                line[x+2] = grayscale; //r
            }
        }

//         nie dziala
//        for(int y=0; y<h;++y){
//            uchar *line = img.scanLine(y);
//            for(int x=0; x<w*4;x+=4){
//                processor.imgdata.image[y*w +x/4][2] = line[x]; //b
//                processor.imgdata.image[y*w +x/4][1] = line[x+1]; //g
//                processor.imgdata.image[y*w +x/4][0] = line[x+2];  //r
//            }
//        }

    }

    return img;
}


void MainWindow::on_actionPrzetworz_wybrane_zdjecie_triggered()
{
    if(!galleryLoaded) return;
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
        currW = imgtab1.width();
        currH = imgtab1.height();
        break;
    case 1:
        currentImg = imgtab2;
        currW = imgtab2.width();
        currH = imgtab2.height();
        break;
    case 2:
        currentImg = imgtab3;
        currW = imgtab3.width();
        currH = imgtab3.height();
        break;
    }
    ui->ImageContainingLabel->setPixmap(QPixmap::fromImage(currentImg));
}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    processRaw(item->text());
}


QImage MainWindow::luminance_histogram()
{
    int w = currentImg.width();
    int h = currentImg.height();

    std::vector<int> luminance(256);

    for(int y=0; y<h;++y){
        uchar *line = currentImg.scanLine(y);
        for(int x=0; x<w*4;x+=4){
            uint pixel_luminance = 0;
            pixel_luminance += static_cast<uchar>(static_cast<double>(line[x]) * 0.0722); //b
            pixel_luminance += static_cast<uchar>(static_cast<double>(line[x+1]) * 0.7152); //g
            pixel_luminance += static_cast<uchar>(static_cast<double>(line[x+2]) * 0.2126); //r
            luminance[pixel_luminance]++;
        }
    }

    int max_luminance = *std::max_element(luminance.begin(), luminance.end());

    for(int i = 0; i < 256; ++i)
        luminance[i] = (static_cast<double>(luminance[i]) / static_cast<double>(max_luminance)) * 256.0;

    QImage img(256, 256, QImage::Format_ARGB32);


    for(int y=0; y<256;++y){
        uchar *line = img.scanLine(y);
        for(int x=0; x<256*4;x+=4){
            if(luminance.at(x/4) <= 256 - y){
                line[x] = 0; //b
                line[x+1] = 0; //g
                line[x+2] = 0; //r
                line[x+3] =255; //alpha
            }
            else{
                line[x] = 255; //b
                line[x+1] = 255; //g
                line[x+2] = 255; //r
                line[x+3] =255; //alpha
            }
        }
    }

    return img;
}
