#include "communicaton.h"

Communicaton::Communicaton(QObject *parent) : QObject(parent)
{
    qslPortList.clear();

}

QStringList Communicaton::GetInfo()
{
    qDebug() << __PRETTY_FUNCTION__;
    qslPortList.clear();
    const auto serialPortInfos = QSerialPortInfo::availablePorts();

    qDebug() << "Total number of ports available: " << serialPortInfos.count() ;

    const QString blankString = "N/A";
    QString description;
    QString manufacturer;
    QString serialNumber;

    for (const QSerialPortInfo &serialPortInfo : serialPortInfos) {
        description = serialPortInfo.description();
        manufacturer = serialPortInfo.manufacturer();
        serialNumber = serialPortInfo.serialNumber();
        qslPortList.append(serialPortInfo.portName());
        qDebug() << endl
            << "Port: " << serialPortInfo.portName() << endl
            << "Location: " << serialPortInfo.systemLocation() << endl
            << "Description: " << (!description.isEmpty() ? description : blankString) << endl
            << "Manufacturer: " << (!manufacturer.isEmpty() ? manufacturer : blankString) << endl
            << "Serial number: " << (!serialNumber.isEmpty() ? serialNumber : blankString) << endl
            << "Vendor Identifier: " << (serialPortInfo.hasVendorIdentifier()
                                         ? QByteArray::number(serialPortInfo.vendorIdentifier(), 16)
                                         : blankString) << endl
            << "Product Identifier: " << (serialPortInfo.hasProductIdentifier()
                                          ? QByteArray::number(serialPortInfo.productIdentifier(), 16)
                                          : blankString) << endl
            << "Busy: " << (serialPortInfo.isBusy() ? "Yes" : "No") << endl;
    }
    return qslPortList;
}

bool Communicaton::OpenConnection(QString portName)
{
    qDebug() << __PRETTY_FUNCTION__;
    if (!qslPortList.contains(portName)){
        qDebug() << "Can't open port "<< portName << " not in Port list";
        return false;
    }
    comPort.setPortName(portName);
    comPort.setBaudRate(115200);
    if (comPort.open(QIODevice::ReadWrite | QIODevice::Text)){
        qDebug() << "Open port "<< portName << " with BaudRate(115200)";
        return true;
    }
    qDebug() << "Can't open port "<< portName;
    return false;
}

bool Communicaton::CloseConnection(QString portName)
{
    if (!qslPortList.contains(portName)){
        qDebug() << "Can't close port "<< portName << " not in Port list";
        return false;
    }
    comPort.setPortName(portName);
    if (comPort.isOpen()){
        qDebug() << "Close port "<< portName;
        comPort.close();
        return true;
    }
    qDebug() << "Can't close port "<< portName;
    return false;
}
