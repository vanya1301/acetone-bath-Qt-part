#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include <QtBluetooth>
#include <vector>
#include <QMessageBox>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->StartStopButton->setEnabled(false);

    socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);

    // temperature = ui->TempSpinBox->value();
    // time = ui->timeSpinBox->value();
    tmr = new QTimer();
    connect(tmr, SIGNAL(timeout()), this, SLOT(updateLabel()));
    connect(socket,SIGNAL(readyRead()),this,SLOT(controllerReader()));

}


MainWindow::~MainWindow()
{
    if(socket->isOpen())
    {
        socket->close();
    }
    delete ui;
}

void MainWindow::on_on_clicked()
{
    socket->write("ON|");
    //QMessageBox::about(this,"","ON button");
}


void MainWindow::on_off_clicked()
{
    socket->write("OFF|");
}

void MainWindow::on_StartStopButton_clicked()
{
    if(ui->StartStopButton->text() == "Start")
    {
        socket->open(QIODevice::ReadWrite);
        if(!tmr->isActive())
            tmr->start(100);
        ui->StartStopButton->setText("Stop");
        command = "T"+QString::number(ui->TempSpinBox->value())+"|";
        socket->write(command.toUtf8());
        command = "D"+QString::number(ui->timeSpinBox->value())+"|";
        socket->write(command.toUtf8());
        runnning = false;
    }
    else
    {
        ui->StartStopButton->setText("Start");
        tmr->stop();
        socket->write("s|");


        runnning = true;
    }
}

void MainWindow::updateLabel()
{

    /*if(socket->isReadable())
    {
        ui->lcdNumber->display(QString::number(socket->QIODevice::bytesAvailable()));
        //receivedData = socket->readAll().toInt();
        //QString str = "JOPA";
        str = socket->readLine(3);


        if(str.toInt()>0)
        {
            ui->lcdNumber->display(str.toInt());
            //ui->SensTempLabel->setText(str);
            // ui->SensTempLabel->setText(str);

        }
        str.clear();
    }*/

    //}
    if(!socket->isOpen())

    {

        //socket->isSignalConnected();
        // socket->

        if(errorShowed)
        {return;}
        QMessageBox::warning(this,"","Connection problem");
        errorShowed = true;
        socket->abort();
        ui->StartStopButton->setText("Start");
        ui->StartStopButton->setEnabled(false);
    }
}

void MainWindow::getAddress(const QString &str)
{
    addressToConnect = "";
    addressToConnect = str;
    connectToDevice(str);
    //QMessageBox::about(this,"",addressToConnect);
}

void MainWindow::controllerReader()
{
    if(socket->isReadable())
    {
        //ui->lcdNumber->display(QString::number(socket->QIODevice::bytesAvailable()));
        //receivedData = socket->readAll().toInt();
        //QString str = "JOPA";
        str = socket->readLine(3);

        /* if(str.startsWith('|'))
        {
            str = str.remove(QRegExp("[|]."));*/
        if(str.toInt()>0)
        {
            ui->lcdNumber->display(str.toInt());
            //ui->SensTempLabel->setText(str);
            // ui->SensTempLabel->setText(str);

        }
        str.clear();
    }
}

void MainWindow::connectToDevice(const QString &str)
{
    static const QString serviceUuid(QStringLiteral("00001101-0000-1000-8000-00805F9B34FB"));
    socket->connectToService(QBluetoothAddress(str), QBluetoothUuid(serviceUuid), QIODevice::ReadWrite);

    if(socket->peerAddress().toString()==str)
    {
        QMessageBox::about(this,"","Device is successfuly connected.\n"+socket->peerName()+"\n"+socket->peerAddress().toString());
        ui->StartStopButton->setEnabled(true);
    }

    else {
        QMessageBox::warning(this,"","Connection error");
    }

}

void MainWindow::on_PairButton_clicked()
{
    BluetoothConnection window;
    connect(&window,SIGNAL(signalAddress(const QString&)),this,SLOT(getAddress(const QString&)));
    window.setModal(true);
    window.showMaximized();
    window.exec();
}
