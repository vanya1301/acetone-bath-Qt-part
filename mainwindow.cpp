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
    ui->pauseButton->setEnabled(false);

    /*gifLbl = new QLabel(this);
    gifLbl->setMovie(gifMovie);
    gifLbl->showMaximized();
    gifMovie->start();*/


    socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);

    tmr = new QTimer();

    connect(tmr, SIGNAL(timeout()), this, SLOT(updateState()));
    connect(socket,SIGNAL(readyRead()),this,SLOT(controllerReader()));
    connect(&window,SIGNAL(signalAddress(const QString&)),this,SLOT(getAddress(const QString&)));
}


MainWindow::~MainWindow()
{
    if(socket->isOpen())
    {
        socket->close();
    }
    delete ui;
}




void MainWindow::on_StartStopButton_clicked(bool checked)
{
    if(!socket->isOpen())
    {
        showError();
        setButtonChecked(false);
        return;
    }
    if(checked)
    {
        qDebug()<<"Checked";
        if(paused)
        {
            socket->write("C|");
            ui->StartStopButton->setIcon(QIcon(":/MyFiles/images/Stop-red.png"));
            ui->pauseButton->setEnabled(true);
            paused = false;
            runnning = true;
            return;
        }

        if(ui->TempSpinBox->value()<1||ui->timeSpinBox->value()<1)
        {
            QMessageBox::warning(this,"","You have to set parametres.\nParametrs can't be equal 0.");
            ui->StartStopButton->setChecked(false);
            return;
        }
        socket->open(QIODevice::ReadWrite);

        if(!tmr->isActive())
        {
            tmr->start(1000);
        }

        setButtonChecked(true);
        ui->pauseButton->setEnabled(true);

        command = "T"+QString::number(ui->TempSpinBox->value())+"|";
        socket->write(command.toUtf8());

        command = "D"+QString::number(ui->timeSpinBox->value())+"|";
        socket->write(command.toUtf8());

        runnning = true;
    }
    else
    {
        qDebug()<<"Not Checked";
        ui->pauseButton->setEnabled(false);
        ui->lcdNumber->display(0);
        ui->lcdNumber_2->display(0);
        socket->write("s|");

        runnning = false;
    }
}

void MainWindow::updateState()
{
    //if()
    qDebug()<<"UPDATE_STATE() "<<errorShowed<<endl;
    //qDebug()<<QBluetoothSocket::ConnectedState<<endl<<endl;
    //{
    if(errorShowed){return;}
    //connectToDevice(addressToConnect);
    if(socket->state()!=QBluetoothSocket::ConnectedState||!socket->isOpen())
    {
        //socket->con
        showError();
    }
    // }
}

void MainWindow::getAddress(const QString &str)
{
    addressToConnect = "";
    addressToConnect = str;
    connectToDevice(addressToConnect);

}

void MainWindow::controllerReader()
{
    receivedInfo = socket->readLine();
    qDebug()<<receivedInfo<<endl;
    if(receivedInfo.startsWith("t"))
    {
        if(!ui->StartStopButton->isEnabled())
        {
            ui->StartStopButton->setEnabled(true);
            showConnected();
            setButtonChecked(true);
            runnning = true;
        }
        if(!ui->StartStopButton->isChecked() && runnning && !paused)
        {
            setButtonChecked(true);//Stop
            ui->StartStopButton->setEnabled(true);
            ui->PairButton->setEnabled(true);
        }
        receivedInfo.remove(QChar('t'), Qt::CaseInsensitive);
        receivedInfo = receivedInfo.trimmed();
        if(receivedInfo.toInt()>0 && runnning)
        {
            ui->lcdNumber->display(receivedInfo.toInt());
        }
    }

    if(receivedInfo.startsWith("m"))
    {
        receivedInfo.remove(QChar('m'), Qt::CaseInsensitive);
        receivedInfo = receivedInfo.trimmed();
        if(receivedInfo.toInt()>60 && runnning){
            ui->lcdNumber_2->display(receivedInfo.toInt()/60+1);
            ui->label_2->setText("min.");
        }
        else if (receivedInfo.toInt()<60 && receivedInfo.toInt()>1 && runnning) {
            ui->lcdNumber_2->display(receivedInfo.toInt());
            ui->label_2->setText("sec.");
        }
        else if(receivedInfo.toInt()<=1){
            //tmr->stop();
            QMessageBox::about(this,"","Proccess is finished!");
            setButtonChecked(false);//Start
            ui->lcdNumber->display(0);
            ui->lcdNumber_2->display(0);
            ui->label_2->setText("min.");
            ui->pauseButton->setEnabled(true);
        }

    }

    if (receivedInfo.startsWith("R"))
    {
        showConnected();
        ui->StartStopButton->setChecked(false);
        if(!tmr->isActive())
        {
            tmr->start(1000);
        }
    }
    receivedInfo.clear();


}
void MainWindow::setButtonChecked(bool a)
{
    if(a){
        ui->StartStopButton->setIcon(QIcon(":/MyFiles/images/Stop-red.png"));
        ui->StartStopButton->setChecked(true);
    }

    else if(!a){
        ui->StartStopButton->setIcon(QIcon(":/MyFiles/images/play-green.png"));
        ui->StartStopButton->setChecked(false);
    }
}

void MainWindow::showError()
{
    QMessageBox::warning(this,"","Connection problem");
    errorShowed = true;
    socket->abort();
    setButtonChecked(false);//Start
    ui->StartStopButton->setEnabled(false);
    ui->pauseButton->setEnabled(false);
    ui->PairButton->setIcon(QIcon(":/MyFiles/images/Bluetooth-512.png"));
    ui->lcdNumber->display(0);
    ui->lcdNumber_2->display(0);
    ui->BluetoothDeviceNameLabel->setText("Reconnect the device ->");
}

void MainWindow::showConnected()
{
    /*gifMovie->stop();
    gifLbl->hide();*/
    QMessageBox::about(this,"","Device is successfuly connected.\n"+socket->peerName()+"\n"+socket->peerAddress().toString());
    ui->StartStopButton->setEnabled(true);
    ui->pauseButton->setEnabled(true);
    ui->BluetoothDeviceNameLabel->setText(socket->peerName());
    ui->PairButton->setIcon(QIcon(":/MyFiles/images/Bluetooth-512-green.png"));
}

void MainWindow::connectToDevice(const QString &str)
{
    qDebug()<<"connectToDevice HERE!!!!"<<endl;
    /*if(socket->isOpen()){
        socket->close();
    }*/

    static const QString serviceUuid(QStringLiteral("00001101-0000-1000-8000-00805F9B34FB"));
    socket->connectToService(QBluetoothAddress(str), QBluetoothUuid(serviceUuid), QIODevice::ReadWrite);
    //QMessageBox::about(this,"","Connecting...");

    /*gifLbl->setScaledContents(true);

    gifLbl->showNormal();
    //gifLbl->show();
    gifMovie->start();*/

}

void MainWindow::on_PairButton_clicked()
{
    if(window.isHidden()){
        window.agentRestart();
        window.setModal(true);
        window.showMaximized();
    }
    else {
        window.setModal(true);
        window.showMaximized();
        window.exec();
    }

}


void MainWindow::on_pauseButton_clicked()
{
    socket->write("P|");
    paused = true;
    runnning = false;
    setButtonChecked(false);
}
