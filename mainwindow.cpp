#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <vector>
#include <QMessageBox>
#include <QDebug>


using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->StartStopButton->setEnabled(false);

    socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);

    tmr = new QTimer();
    connect(tmr, SIGNAL(timeout()), this, SLOT(updateState()));
    connect(socket,SIGNAL(readyRead()),this,SLOT(controllerReader()));
    connect(&window,SIGNAL(signalAddress(const QString&)),this,SLOT(getAddress(const QString&)));


    //tmr->1

}


MainWindow::~MainWindow()
{
    if(socket->isOpen())
    {
        socket->close();
    }
    delete ui;
}



void MainWindow::on_StartStopButton_clicked()
{
    if(ui->StartStopButton->text() == "Start")
    {
        if(ui->TempSpinBox->value()<1||ui->timeSpinBox->value()<1)
        {
            QMessageBox::warning(this,"","You have to set parametres.\nParametrs can't be equal 0.");
            return;
        }
        socket->open(QIODevice::ReadWrite);
        if(!tmr->isActive())
        {
            tmr->start(1000);
        }

        ui->StartStopButton->setText("Stop");

        command = "T"+QString::number(ui->TempSpinBox->value())+"|";
        socket->write(command.toUtf8());

        command = "D"+QString::number(ui->timeSpinBox->value())+"|";
        socket->write(command.toUtf8());

        runnning = true;
    }
    else
    {
        ui->StartStopButton->setText("Start");
        ui->lcdNumber->display(0);
        ui->lcdNumber_2->display(0);

        if(tmr->isActive())
        {
            tmr->stop();
        }

        socket->write("s|");

        runnning = false;
    }
}

void MainWindow::updateState()
{
    // qDebug()<<socket->state();
    if(!socket->isOpen())
    {
        if(errorShowed){return;}
        //connectToDevice(addressToConnect);
        if(socket->state()!=QBluetoothSocket::ConnectedState||socket->peerAddress().toString()!=addressToConnect)
        {
            if(socket->QBluetoothSocket::ConnectedState)
            //QBluetoothSocket::ConnectedState;
            QMessageBox::warning(this,"","Connection problem");
            errorShowed = true;
            socket->abort();
            ui->StartStopButton->setText("Start");
            ui->StartStopButton->setEnabled(false);
        }
    }
}

void MainWindow::getAddress(const QString &str)
{
    addressToConnect = "";
    addressToConnect = str;
    connectToDevice(addressToConnect);

}

void MainWindow::controllerReader()
{
    // if(socket->isReadable())
    //{
    receivedInfo = socket->readLine();
    qDebug()<<receivedInfo<<endl;
    if(receivedInfo.startsWith("t"))
    {
        if(ui->StartStopButton->text()=="Start" && runnning)
        {
            ui->StartStopButton->setText("Stop");
            ui->StartStopButton->setEnabled(true);
        }
        receivedInfo.remove(QChar('t'), Qt::CaseInsensitive);
        receivedInfo = receivedInfo.trimmed();
        if(receivedInfo.toInt()>0)
        {
            ui->lcdNumber->display(receivedInfo.toInt());
        }
    }

    else if(receivedInfo.startsWith("m"))
    {
        receivedInfo.remove(QChar('m'), Qt::CaseInsensitive);
        receivedInfo = receivedInfo.trimmed();
        if(receivedInfo.toInt()>60){
            ui->lcdNumber_2->display(receivedInfo.toInt()/60+1);
            ui->label_2->setText("min.");
        }
        else if (receivedInfo.toInt()<60 && receivedInfo.toInt()>=5) {
            ui->lcdNumber_2->display(receivedInfo.toInt());
            ui->label_2->setText("sec.");
        }
        else if(receivedInfo.toInt()<=2){
            tmr->stop();
            QMessageBox::about(this,"","Proccess is finished!");
            ui->StartStopButton->setText("Start");
            ui->lcdNumber->display(0);
            ui->lcdNumber_2->display(0);
            ui->label_2->setText("min.");
        }

    }

    else if (receivedInfo.startsWith("R"))
    {
        QMessageBox::about(this,"","Device is successfuly connected.\n"+socket->peerName()+"\n"+socket->peerAddress().toString());
        ui->StartStopButton->setEnabled(true);
    }
    /*if(receivedInfo.toInt()>0)
        {
            ui->lcdNumber->display(receivedInfo.toInt());
        }*/
    receivedInfo.clear();
    // }
}

void MainWindow::connectToDevice(const QString &str)
{
    qDebug()<<"connectToDevice HERE!!!!"<<endl;
    static const QString serviceUuid(QStringLiteral("00001101-0000-1000-8000-00805F9B34FB"));
    socket->connectToService(QBluetoothAddress(str), QBluetoothUuid(serviceUuid), QIODevice::ReadWrite);
    QMessageBox::about(this,"","Connecting...");
    // QMessageBox::
    /* while(!socket->readLine().startsWith("R"))
    {

}*/

    /*if(socket->peerAddress().toString()==str)
    {
        QMessageBox::about(this,"","Device is successfuly connected.\n"+socket->peerName()+"\n"+socket->peerAddress().toString());
        //ui->StartStopButton->setEnabled(true);
    }

    else {
    QMessageBox::warning(this,"","Connection error");
}*/

}

void MainWindow::on_PairButton_clicked()
{
    if(socket->isOpen())
        socket->close();





    window.setModal(true);
    window.showMaximized();
    window.exec();
}
