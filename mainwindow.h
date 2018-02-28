#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <opencv/cv.hpp>
#include <opencv/highgui.h>

#include <QTimer>

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
    void displayFrame();

private:
    Ui::MainWindow *ui;
    cv::Mat left;
    cv::Mat right;
    VideoCapture cam;
    VideoCapture cam2;

};

#endif // MAINWINDOW_H
