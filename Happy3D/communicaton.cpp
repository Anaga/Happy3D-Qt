#include "communicaton.h"

Communicaton::Communicaton(QObject *parent) : QObject(parent)
{
    m_portList.clear();
   // connect(m_serialPort, &QSerialPort::readyRead, this, &Communicaton::handleReadyRead);
    connect(m_serialPort, &QSerialPort::errorOccurred, this, &Communicaton::handleError);
    connect(&m_timer, &QTimer::timeout, this, &Communicaton::handleTimeout);

    connect(&comPort, SIGNAL( readyRead() ), this, SLOT( getData() ) );

    //m_timer.start(5000);

}


void Communicaton::handleReadyRead()
{
    //qDebug() << "New data!";
    m_readData.append(m_serialPort->readAll());
    if (!m_timer.isActive())
        m_timer.start(500);
}

void Communicaton::handleTimeout()
{
    if (!m_readData.isEmpty()) {
         //qDebug() << QObject::tr("Data successfully received from port %1")
         //                   .arg(m_serialPort->portName());

         qDebug() << m_readData << endl;
         emit readDataFromCom(m_readData);
         m_readData.clear();
    }


}

void Communicaton::handleError(QSerialPort::SerialPortError serialPortError)
{
    if (serialPortError == QSerialPort::ReadError) {
         qDebug() << QObject::tr("An I/O error occurred while reading "
                                        "the data from port %1, error: %2")
                            .arg(m_serialPort->portName())
                            .arg(m_serialPort->errorString())
                         << endl;
    }
}

void Communicaton::getData()
{
    m_readData.append(m_serialPort->readAll());
    if (!m_timer.isActive())
        m_timer.start(100);
}

QStringList Communicaton::GetInfo()
{
    qDebug() << __PRETTY_FUNCTION__;
    m_portList.clear();
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
        m_portList.append(serialPortInfo.portName());
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
    return m_portList;
}

bool Communicaton::OpenConnection(QString portName)
{
    qDebug() << __PRETTY_FUNCTION__;
    QString qsTemp = "Failed to open port %1, error: %2";

/*
    QSerialPort serialPort;
    const QString serialPortName = portName;
    serialPort.setPortName(serialPortName);
    serialPort.setBaudRate(QSerialPort::Baud9600);

    if (!serialPort.open(QIODevice::ReadWrite)) {
        qsTemp = qsTemp.arg(serialPortName)
                       .arg(serialPort.errorString());
        qDebug() << qsTemp;
        return false;
    }
    qDebug() << "Open port "<< portName << " with Baud9600";
    return true;
*/
    if (!m_portList.contains(portName)){
        qDebug() << "Can't open port "<< portName << " not in Port list";
        return false;
    }
    comPort.setPortName(portName);
    comPort.setBaudRate(115200);
    comPort.setBaudRate(9600);
    if (comPort.open(QIODevice::ReadWrite)){
        qDebug() << "Open port "<< portName << " with BaudRate(115200)";
        m_serialPort = &comPort;
        return true;
    }
    qDebug() << "Can't open port "<< portName;
    return false;
}

bool Communicaton::CloseConnection(QString portName)
{
    qDebug() << __PRETTY_FUNCTION__;
    if (!m_portList.contains(portName)){
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

bool Communicaton::SendCommand(QString portName, QString command)
{
    qDebug() << __PRETTY_FUNCTION__;
    comPort.setPortName(portName);
    if (!comPort.isOpen()){
        qDebug() << "Can't send command"<<command <<"to port"<< portName << ", port not open";
        return false;
    }

    m_writeData = command.toLocal8Bit();
    const qint64 bytesWritten = comPort.write(m_writeData);

    if (bytesWritten == -1) {
        qDebug() << "Failed to write the data to port "<< portName << "error: "<< comPort.errorString();
        return false;
    } else if (bytesWritten != m_writeData.size()) {
        qDebug() << "Failed to write all the data to port "<< portName << "error: "<< comPort.errorString();
        return false;
    }

    qDebug() << "Command"<<command <<" successfully sent to port "<< portName;
    return true;
}
