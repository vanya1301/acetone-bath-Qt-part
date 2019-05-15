#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothSocket>
#include <QListWidget>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QTextEdit>
#include <string>
#include <QTimer>
#include "bluetoothconnection.h"
//#include <QLCDNumber>



namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_on_clicked();
    void on_off_clicked();
    void on_StartStopButton_clicked();
    void updateLabel();
    void getAddress(const QString& str);
    void controllerReader();
    void on_PairButton_clicked();

private:
    void connectToDevice(const QString& str);
    Ui::MainWindow *ui;
    QBluetoothSocket *socket;
    QString addressToConnect;
    QString command;
    QString str="";
    QTimer *tmr;
    bool errorShowed =false;
    int receivedData;
   // int temperature;
   // int time;
    bool runnning=false;  
};


#endif // MAINWINDOW_H
