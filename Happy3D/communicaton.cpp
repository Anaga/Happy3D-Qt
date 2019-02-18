#include "communicaton.h"

Communicaton::Communicaton(QObject *parent) : QObject(parent)
{
    qslRet.clear();

}

QStringList Communicaton::GetInfo()
{
    qslRet.clear();
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
        qslRet.append(serialPortInfo.portName());
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
    return qslRet;
}
