#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cam=VideoCapture(0);
    cam2=VideoCapture(1);

    QTimer *qTimer=new QTimer(this);
    connect(qTimer,SIGNAL(timeout()),this,SLOT(displayFrame()));
    qTimer->start(10);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::displayFrame() {
    Mat leftIn, rightIn;
    cam>>leftIn;
    cam2>>rightIn;

    cv::cvtColor(leftIn, left, CV_BGR2RGB);
    cv::cvtColor(rightIn, right, CV_BGR2RGB);

    QImage leftIm = QImage((const unsigned char*)(left.data),left.cols,left.rows,
                         left.step,QImage::Format_RGB888);

    ui->label->setPixmap(QPixmap(QPixmap::fromImage(leftIm)));
    QImage rightIm = QImage((const unsigned char*)(right.data),right.cols,right.rows,
                         right.step,QImage::Format_RGB888);

    ui->label_2->setPixmap(QPixmap(QPixmap::fromImage(rightIm)));
}
