#ifndef BLUETOOTHCONNECTION_H
#define BLUETOOTHCONNECTION_H

#include <QDialog>
#include <QBluetoothDeviceDiscoveryAgent>
#include <vector>
#include <QListWidget>


namespace Ui {
class BluetoothConnection;
}

class BluetoothConnection : public QDialog
{
    Q_OBJECT

public:
    explicit BluetoothConnection(QWidget *parent = nullptr);
    ~BluetoothConnection();

    QString addressToConnect;
signals:
    void signalAddress(const QString&);
private slots:
    void deviceDiscovered(const QBluetoothDeviceInfo &device);


    void on_exitButton_clicked();

    void on_connectButton_clicked();

    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_refreshButton_clicked();

private:
    void sendAddress();

    Ui::BluetoothConnection *ui;
    QBluetoothDeviceDiscoveryAgent *agent = new QBluetoothDeviceDiscoveryAgent;
    struct BlDevice{QString name;QString address;};
    std::vector<BlDevice> devices;

};

#endif // BLUETOOTHCONNECTION_H
