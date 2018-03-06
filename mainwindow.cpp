#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <climits>

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

    if (diffSet) {
        int disparityMap[left.rows][left.cols];
        int PATCH_WIDTH = 3;
        int PATCH_HEIGHT = 3;
        int maxDistance = -1;

        for (int row = 0; row < left.rows-PATCH_HEIGHT; row+=PATCH_HEIGHT) {
            for (int col = 0; col < left.cols-PATCH_WIDTH; col+=PATCH_WIDTH) {
                int bestMatchRight = findBestMatch(left, right, col, row, PATCH_WIDTH, PATCH_HEIGHT);
                int colDiff = bestMatchRight - col;
                if (colDiff > maxDistance) maxDistance = colDiff;
                for (int patchRow=0; patchRow<PATCH_HEIGHT; patchRow++)
                    for (int patchCol = 0; patchCol<PATCH_HEIGHT; patchCol++)
                        if (bestMatchRight != -1)
                            disparityMap[patchRow][patchCol] = colDiff;
                        else
                            disparityMap[patchRow][patchCol] = INT_MAX;
            }
        }

        maxDistance = maxDistance * maxDistance;

        Mat disparityView = Mat(left.rows,left.cols,left.type());
        for (int row = 0; row < disparityView.rows; row++) {
            Vec3b* pixel = disparityView.ptr<Vec3b>(row);
            for (int col = 0; col < disparityView.cols; col++) {
                int normalizedDiff = ((double)((disparityMap[row][col])*(disparityMap[row][col])) / maxDistance) * 255;
                pixel[0] = normalizedDiff;
                pixel[1] = 0;
                pixel[2] = 0;
                pixel++;
            }
        }

        QImage disparityIm = QImage((const unsigned char*)(disparityView.data),disparityView.cols,disparityView.rows,
                                    disparityView.step,QImage::Format_RGB888);
        ui->label_3->setPixmap(QPixmap(QPixmap::fromImage(disparityIm)));
    }
}

void MainWindow::on_pushButton_clicked()
{
    int leftRow = -1;
    int rightRow = -1;
    for (int row = 0; row < left.rows; row++) {
        if (leftRow == -1) {
            cv::Vec3b pixel = left.at<Vec3b>(row, 0);
            if (pixel[0] < 50 && pixel[1] < 50 && pixel[2] < 50)
                leftRow = row;
        }
        if (rightRow == -1) {
            cv::Vec3b pixel = right.at<Vec3b>(row, 0);
            if (pixel[0] < 50 && pixel[1] < 50 && pixel[2] < 50)
                rightRow = row;
        }
    }

    rowDiff = rightRow - leftRow;
    diffSet = true;
}




int MainWindow::findBestMatch(Mat& orig, Mat& other, int startX, int startY, int patchWidth, int patchHeight) {
    int minSquareDiff = -1;
    int minSquareDiffCol = -1;

    int otherRow = startY + rowDiff;
    if (otherRow + patchHeight > other.rows - 1 || otherRow < 0) return -1;
    for (int col = 0; col < other.cols - patchWidth; col++) {
        int squareDiff = sumSquareDiff(orig, other, startX, startY, col, otherRow, patchWidth, patchHeight);
        if (squareDiff < minSquareDiff || minSquareDiff == -1) {
            minSquareDiff = squareDiff;
            minSquareDiffCol = col;
        }
    }

    return minSquareDiffCol;
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
