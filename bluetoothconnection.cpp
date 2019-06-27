#include "bluetoothconnection.h"
#include "ui_bluetoothconnection.h"
#include <QtBluetooth>
#include <QMessageBox>
#include <QString>


BluetoothConnection::BluetoothConnection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BluetoothConnection)
{
    ui->setupUi(this);

    connect(agent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),
            this, SLOT(deviceDiscovered(QBluetoothDeviceInfo)));

    agent->stop();
    agent->start();
    ui->connectButton->setEnabled(false);
    addressToConnect = "";

}

BluetoothConnection::~BluetoothConnection()
{
    agent->stop();
    delete ui;
}

void BluetoothConnection::agentRestart()
{
    agent->stop();
    ui->listWidget->clear();
    ui->connectButton->setEnabled(false);
    devices.clear();
    agent->start();
}


void BluetoothConnection::deviceDiscovered(const QBluetoothDeviceInfo &device)
{
    discovered.name=device.name();
    discovered.address=device.address().toString();
    devices.insert(devices.end(),discovered);
    ui->listWidget->addItem(discovered.name);
}

void BluetoothConnection::on_connectButton_clicked()
{
    sendAddress();
    this->hide();

}
void BluetoothConnection::on_exitButton_clicked()
{
    this->hide();
}



void BluetoothConnection::on_listWidget_itemClicked(QListWidgetItem *item)
{
    ui->connectButton->setEnabled(true);
    for(const auto& i:devices)
    {
        if(i.name==item->text())
            BluetoothConnection::addressToConnect=i.address;
    }
}

void BluetoothConnection::sendAddress()
{
    emit signalAddress(addressToConnect);
}



void BluetoothConnection::on_refreshButton_clicked()
{
    agentRestart();
}
