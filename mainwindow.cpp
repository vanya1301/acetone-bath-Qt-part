#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <vector>
#include <QDebug>
//#include <QLabel>



using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->StartStopButton->setEnabled(false);
    ui->pauseButton->setEnabled(false);

    window.setStyleSheet("QLabel{ rgb(255, 255, 0)}");

    //window-BluetoothConnection(this);
    /*gifLbl = new QLabel(this);
    gifLbl->setMovie(gifMovie);
    gifLbl->showMaximized();
    gifMovie->start();*/

    socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);

    tmr = new QTimer();

    /*orange_spin->setBackgroundColor(QColor(0,0,0,255));
    QFont font = QFont("URW Gothic L",40,-1,true);
    font.setBold(true);
    tempSens->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Maximum);
    tempSens->setFont(font);
    tempSens->setStyleSheet("color:rgb(255,255,255);\nbackground-color:transparent;");
    //tempSens->setBackgroundRole(QPalette().setBrush();

   //orange_spin->setScaledSize(QSize(100,100));

    //ui->tempSensLabel->setMovie(orange_spin);
    ui->tempSensLabel->setPixmap(QPixmap(":/MyFiles/images/InterfaceImages/orange-circle.png"));
    orange_spin->start();
    ui->tempSensLabel->setLayout(new QHBoxLayout());
    ui->tempSensLabel->layout()->addWidget(tempSens);
    ui->tempSensLabel->layout()->addWidget(ui->label);
    tempSens->setText("0");*/

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
            qDebug()<<"WAS UNPAUSED"<<endl;
            socket->write("C|");
            setButtonChecked(true);
            ui->pauseButton->setEnabled(true);
            paused = false;

            ui->pauseButton->setIcon((QIcon(":/MyFiles/images/InterfaceImages/pause na.png")));
            runnning = true;
            return;
        }

        if(ui->TempSpinBox->value()<1||ui->timeSpinBox->value()<1)
        {
            msg.warning(this,"","You have to set parametres.<br>Parametrs can't be equal 0.");
            ui->StartStopButton->setChecked(false);
            return;
        }

        command = "T"+QString::number(ui->TempSpinBox->value())+"|";
        socket->write(command.toUtf8());

        command = "D"+QString::number(ui->timeSpinBox->value())+"|";
        socket->write(command.toUtf8());

        if(!tmr->isActive())
        {
            tmr->start(1000);
        }

        runnning = true;
        setButtonChecked(true);
        ui->pauseButton->setEnabled(true);
    }
    else
    {
        qDebug()<<"Not Checked";
        ui->pauseButton->setEnabled(false);
        //ui->lcdNumber->display(0);
        if(ui->tempSensLabel->text()!="0")
        ui->tempSensLabel->setText("0");

        if(ui->procTimeLabel->text()!="0")
            ui->procTimeLabel->setText("0");

        //ui->lcdNumber_2->display(0);
        if(ui->procTimeLabel->text()!="0")
            ui->procTimeLabel->setText("0");

        socket->write("s|");
        setButtonChecked(false);
        runnning = false;
    }
}

void MainWindow::updateState()
{

    qDebug()<<"UPDATE_STATE() "<<errorShowed<<endl;

    if(errorShowed){return;}
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
            //ui->lcdNumber->display(receivedInfo.toInt());
            if(ui->tempSensLabel->text()!=receivedInfo)
                ui->tempSensLabel->setText(receivedInfo);

        }
    }

    if(receivedInfo.startsWith("m") )
    {
        receivedInfo.remove(QChar('m'), Qt::CaseInsensitive);
        receivedInfo = receivedInfo.trimmed();
        if(receivedInfo.toInt()>60 && runnning){
            //ui->lcdNumber_2->display(receivedInfo.toInt()/60);
            if(ui->procTimeLabel->text()!=receivedInfo)
                ui->procTimeLabel->setText(receivedInfo);

            ui->label_2->setText("min.");
        }
        else if (receivedInfo.toInt()<60 && runnning) {
            //ui->lcdNumber_2->display(receivedInfo.toInt());
            if(ui->procTimeLabel->text()!=receivedInfo)
                ui->procTimeLabel->setText(receivedInfo);
            ui->label_2->setText("sec.");
        }
    }
    if(receivedInfo.startsWith("f")){

        msg.about(this,"","Proccess is finished!");

        setButtonChecked(false);//Start
        //ui->lcdNumber->display(0);
        if(ui->tempSensLabel->text()!="0")
            ui->tempSensLabel->setText("0");

        //ui->lcdNumber_2->display(0);
        if(ui->procTimeLabel->text()!="0")
            ui->procTimeLabel->setText("0");

        ui->label_2->setText("min.");
        ui->pauseButton->setEnabled(true);
    }

    if (receivedInfo.startsWith("R"))
    {
        if(receivedInfo.contains('P'))
        {
            qDebug()<<"PAUSED"<<endl;
            paused = true;
            ui->pauseButton->setIcon((QIcon(":/MyFiles/images/InterfaceImages/pause.png")));
            runnning = false;
            setButtonChecked(false);
        }
        qDebug()<<"READY"<<endl;
        showConnected();
        setButtonChecked(false);

        if(!tmr->isActive())
        {
            tmr->start(1000);
        }
    }

    receivedInfo.clear();


}
void MainWindow::setButtonChecked(bool a)
{
    if(a == true){
        ui->StartStopButton->setIcon(QIcon(":/MyFiles/images/InterfaceImages/stop.png"));
        if(!ui->StartStopButton->isChecked())
            ui->StartStopButton->setChecked(true);
    }

    else if(a == false){
        ui->StartStopButton->setIcon(QIcon(":/MyFiles/images/InterfaceImages/play.png"));
        ui->StartStopButton->setChecked(false);
    }
}

void MainWindow::showError()
{
    msg.warning(this,"","Connection problem");
    errorShowed = true;
    socket->abort();
    setButtonChecked(false);//Start
    ui->StartStopButton->setEnabled(false);
    ui->pauseButton->setEnabled(false);

    ui->PairButton->setIcon(QIcon(":/MyFiles/images/InterfaceImages/Bluetooth na.png"));
    //ui->lcdNumber->display(0);
    if(ui->tempSensLabel->text()!="0")
        ui->tempSensLabel->setText("0");

    //ui->lcdNumber_2->display(0);
    if(ui->procTimeLabel->text()!="0")
        ui->procTimeLabel->setText("0");

    ui->BluetoothDeviceNameLabel->setText("Connection lost");
}

void MainWindow::showConnected()
{
    /*gifMovie->stop();
    gifLbl->hide();*/
    msg.about(this,"","Device is successfuly connected.<br>"+socket->peerName()+"<br>"+socket->peerAddress().toString());


    //"Device is successfuly connected.\n"+socket->peerName()+"\n"+socket->peerAddress().toString()

    ui->StartStopButton->setEnabled(true);
    ui->pauseButton->setEnabled(true);
    ui->BluetoothDeviceNameLabel->setText(socket->peerName());
    ui->PairButton->setIcon(QIcon(":/MyFiles/images/InterfaceImages/Bluetooth.png"));
    errorShowed = false ;
    socket->open(QIODevice::ReadWrite);
}

void MainWindow::connectToDevice(const QString &str)
{
    qDebug()<<"connectToDevice HERE!!!!"<<endl;

    static const QString serviceUuid(QStringLiteral("00001101-0000-1000-8000-00805F9B34FB"));
    socket->connectToService(QBluetoothAddress(str), QBluetoothUuid(serviceUuid), QIODevice::ReadWrite);
}

void MainWindow::on_PairButton_clicked()
{

    if(window.isHidden()){
        window.agentRestart();
        window.setModal(true);
        window.showMaximized();
        //window.showNormal();

    }
    else {
        window.setModal(true);
        window.showMaximized();
        //window.showNormal();
        window.exec();
    }

}


void MainWindow::on_pauseButton_clicked()
{
    socket->write("P|");
    paused = true;
    ui->pauseButton->setIcon((QIcon(":/MyFiles/images/InterfaceImages/pause.png")));
    runnning = false;
    setButtonChecked(false);
}
