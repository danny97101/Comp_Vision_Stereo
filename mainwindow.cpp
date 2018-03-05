#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cam=cv::VideoCapture(0);
    cam2=cv::VideoCapture(1);

    QTimer *qTimer=new QTimer(this);
    connect(qTimer,SIGNAL(timeout()),this,SLOT(displayFrame()));
    qTimer->start(10);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::displayFrame() {
    cv::Mat leftIn, rightIn;
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

void MainWindow::on_pushButton_clicked()
{
    int leftRow = -1;
    int rightRow = -1;
    for (int row = 0; row < left.rows; row++) {
        if (leftRow == -1) {
            cv::Vec3b pixel = left.at<Vec3b>(row, 0);
            if (pixel[0] < 15 && pixel[1] < 15 && pixel[2] < 15)
                leftRow = row;
        }
        if (rightRow == -1) {
            cv::Vec3b pixel = right.at<Vec3b>(row, 0);
            if (pixel[0] < 15 && pixel[1] < 15 && pixel[2] < 15)
                rightRow = row;
        }
    }

    rowDiff = leftRow - rightRow;
}


int MainWindow::findBestMatch(Mat& orig, Mat& other, int startX, int startY, int patchWidth, int patchHeight) {

}

int MainWindow::sumSquareDiff(Mat& orig, Mat& other, int origX, int origY, int otherX, int otherY, int patchWidth, int patchHeight) {
    int sum = 0;
    for (int row = 0; row < patchHeight; row++) {
        for (int col = 0; col < patchWidth; col++) {
            Vec3b origPixel = orig.at<Vec3b>(origY + row, origX + col);
            Vec3b otherPixel = other.at<Vec3b>(otherY + row, otherX + col);
            for (int color = 0; color < 3; color++) {
                sum += (origPixel[color] - otherPixel[color]) * (origPixel[color] - otherPixel[color]);
            }
        }
    }
    return sum;
}
