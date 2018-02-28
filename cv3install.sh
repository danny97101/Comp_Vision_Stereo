#!/bin/bash

#install openCV
cd
cd Downloads
wget http://downloads.sourceforge.net/project/opencvlibrary/opencv-unix/3.4.0/opencv-3.4.0.zip
unzip opencv-3.4.0.zip
cd opencv-3.4.0/
mkdir release
cd release
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local ..
make
sudo make install
sudo ldconfig

