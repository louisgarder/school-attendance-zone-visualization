#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "fstream"
#include <QApplication>
#include <QPainter>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


/*
 * This function reads the data from the input file and plots the data as
 * Qpoints. Each group block is represented by a single point, plotted at the longitude and
 * latitude of the group block's center of population. This function contians two variables,
 * inputFile and configuration, that need to be initialized by the user.
 */

void MainWindow::paintEvent(QPaintEvent* event){

    QPainter painter(this);

    //Input file should be in exact format as the output file of the School Attendance Zone project (https://github.com/louisgarder/school-atendance-zones)
    //A sample file is shown in "MaricopaCountyOutputFile.txt," which contains two configurations.
    string inputFile = "MaricopaCountyOutputFile.txt";

    //Set this integer value equal to the specific conifiguration from the inputFile that should be displayed
    int configuration = 2;


    //Group blocks are represented by the longitude and latitude of their center of population, which is represented by a pair<double, double>
    //vector<pair<double,double>> represents a school attendance zone of group blcoks
    //vector<vector<pair<double,double>>> represents a complete onfiguration of school attendance zone
    vector<vector<pair<double, double>>> coordinates;

    ifstream in(inputFile);
    string str;
    bool configurationFound = false;

    //transfers data from the inputFile to the vector 'coordinates'
    while (getline(in, str)){
        if(str.substr(0,15) == "Configuration " + to_string(configuration)){
            int attendanceZone = 0;
            while (getline(in, str)){
                if(str.empty())
                    break;
                if(str.substr(0,23) == "\tSchool Attendance Zone"){
                    vector<pair<double, double>> newAttendanceZone;
                    coordinates.push_back(newAttendanceZone);
                    attendanceZone++;
                }
                else{
                    int longitudeIndex = str.find("Longitude");
                    int latitudeIndex = str.find("Latitude");
                    if(longitudeIndex != -1){
                        double longitude = stod(str.substr(longitudeIndex + 11, latitudeIndex - longitudeIndex - 12));
                        double latitude = stod(str.substr(latitudeIndex+10));
                        pair<double, double> groupBlock (longitude, latitude);
                        coordinates[attendanceZone-1].push_back(groupBlock);
                    }
                }
            }
            configurationFound = true;
            break;
        }
    }

    if(configurationFound == false){
        QString message = "Configuration was not found. Please set 'configuraton' to a valid value.";
        painter.drawText(QPoint(50,50), message);
    }

    //Finding the minimum and maximum longitudes and latitudes of all group bocks.
    double minLongitude = coordinates[0][0].first;
    double maxLongitude = coordinates[0][0].first;
    double minLatitude = coordinates[0][0].second;
    double maxLatitude =coordinates[0][0].second;
    for(int x = 0; x < (int) coordinates.size(); x++){
        for(int y = 0; y < (int) coordinates[x].size(); y++){
            if(coordinates[x][y].first < minLongitude){
                minLongitude = coordinates[x][y].first;
            }
            if(coordinates[x][y].first > maxLongitude){
                maxLongitude = coordinates[x][y].first;
            }
            if(coordinates[x][y].second < minLatitude){
                minLatitude = coordinates[x][y].second;
            }
            if(coordinates[x][y].second > maxLatitude){
                maxLatitude = coordinates[x][y].second;
            }
        }
    }



    //Sets the length and height of the coordinate plot to specific values to
    //ensure only minimal stretch/compression
    int length = 0;
    int height = 0;
    if((maxLongitude - minLongitude) >= (maxLatitude - minLatitude)){
        length = 1000;
        height = length * (double)(maxLatitude - minLatitude) / (maxLongitude - minLongitude);
    }
    else{
        height = 1000;
        length = height * (double)(maxLongitude - minLongitude) / (maxLatitude - minLatitude);
    }


    painter.setBrush(Qt::SolidPattern);
    painter.setBrush(Qt::white);
    QPen pen;
    pen.setWidth(3);
    painter.setPen(pen);
    painter.drawRect(QRect(10, 10, length + 10, height + 10 ));

    //Goes through each attendance zone and plots each group block
    for(int x = 0; x < (int) coordinates.size(); x++){
        //Each zone is drawn with a random color
        int r = rand() % 256;
        int g = rand() % 256;
        int b = rand() % 256;
        QColor color(r,g,b, 255);
        pen.setColor(color);
        painter.setPen(pen);
        for(int y = 0; y < (int) coordinates[x].size(); y++){
            int xCoordinate =  (int) ((coordinates[x][y].first - minLongitude) / (maxLongitude - minLongitude) * (double) length + 15);
            int yCoordinate = (double) (coordinates[x][y].second - minLatitude) / (maxLatitude - minLatitude) * height + 15;
            painter.drawPoint(QPoint(xCoordinate, yCoordinate));
        }
    }
}

