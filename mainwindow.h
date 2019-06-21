#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#include <QtAndroidExtras>
#include <QMainWindow>
#include <QBluetoothSocket>
#include <QListWidget>
#include <QTextEdit>
#include <string>
#include <QTimer>
#include <QElapsedTimer>
#include "bluetoothconnection.h"
//#include <QLCDNumber>
#include <QMessageBox>
#include <QLabel>
#include <QMovie>



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
   // void on_StartStopButton_clicked(bool checked);
    void updateState();
    void getAddress(const QString& str);
    void controllerReader();
    void on_PairButton_clicked();

    void on_StartStopButton_clicked(bool checked);

    void on_pauseButton_clicked();

private:
    void connectToDevice(const QString& str);
    void setButtonChecked(bool a);
    void showError();
    void showConnected();
    Ui::MainWindow *ui;
    QMessageBox mes;
    BluetoothConnection window;
    QBluetoothSocket *socket;
    QString addressToConnect;
    QString command;
    QString receivedInfo="";
    QTimer *tmr;
    QLabel *gifLbl;
    QMovie *gifMovie = new QMovie(":/MyFiles/gifs/ajax-loader2.gif");
    bool errorShowed =false;
    bool paused = false;
    //bool finishedShowed = false;
   // int temperature;
   // int time;
    bool runnning=false;


};


#endif // MAINWINDOW_H
