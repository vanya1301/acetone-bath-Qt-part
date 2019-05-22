#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#include <QtAndroidExtras>
#include <QMainWindow>
//#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothSocket>
#include <QListWidget>
#include <QTextEdit>
#include <string>
#include <QTimer>
#include <QElapsedTimer>
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
    void on_StartStopButton_clicked();
    void updateState();
    void getAddress(const QString& str);
    void controllerReader();
    void on_PairButton_clicked();

private:
    void connectToDevice(const QString& str);
    Ui::MainWindow *ui;
    BluetoothConnection window;
    QBluetoothSocket *socket;
    QString addressToConnect;
    QString command;
    QString receivedInfo="";
    QTimer *tmr;
    bool errorShowed =false;
   // int temperature;
   // int time;
    bool runnning=false;


};


#endif // MAINWINDOW_H
