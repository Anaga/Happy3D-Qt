#include "communicaton.h"

Communicaton::Communicaton(QObject *parent, QString name) : QObject(parent)
{
    m_portList.clear();
   // connect(m_serialPort, &QSerialPort::readyRead, this, &Communicaton::handleReadyRead);
    connect(m_serialPort, &QSerialPort::errorOccurred, this, &Communicaton::handleError);
    connect(&m_timer, &QTimer::timeout, this, &Communicaton::handleTimeout);

    connect(&comPort, SIGNAL( readyRead() ), this, SLOT( getData() ) );
    qsTemp = "Comm %1";
    qsTemp = qsTemp.arg(name);
    _logger = spdlog::daily_logger_mt(qPrintable(qsTemp), "logs/logfile.log", 8, 00); // new log on each morning at 8:00

    _logger->info("Communicaton startup");

    //m_timer.start(5000);

}

Communicaton::~Communicaton()
{
    _logger->info("Communicaton shutdown");
}




void Communicaton::handleReadyRead()
{
    //qDebug() << "New data!";
    m_readData.append(m_serialPort->readAll());
    if (!m_timer.isActive())
        m_timer.start(50);
}

void Communicaton::handleTimeout()
{
    if (!m_readData.isEmpty()) {
         //qDebug() << QObject::tr("Data successfully received from port %1")
         //                   .arg(m_serialPort->portName());

         //qDebug() << m_readData << endl;
        _logger->info("handleTimeout {}",m_readData);
         emit readDataFromCom(m_readData);
         m_readData.clear();
    }


}

void Communicaton::handleError(QSerialPort::SerialPortError serialPortError)
{
    if (serialPortError == QSerialPort::ReadError) {
        _logger->error("An I/O error occurred while reading "
                         "the data from port {}, error: {}",
                       qPrintable(m_serialPort->portName()),
                       qPrintable(m_serialPort->errorString()));
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
        m_timer.start(10);
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

    if (!m_portList.contains(portName)){
        qDebug() << "Can't open port "<< portName << " not in Port list";
        return false;
    }
    comPort.setPortName(portName);
    comPort.setBaudRate(QSerialPort::Baud115200);
    comPort.setDataBits(QSerialPort::Data8);
    comPort.setParity(QSerialPort::NoParity);
    comPort.setStopBits(QSerialPort::OneStop);
    comPort.setFlowControl(QSerialPort::SoftwareControl);
    //comPort.setBaudRate(9600);
    if (comPort.open(QIODevice::ReadWrite)){
        qDebug() << "Open port "<< portName << " with BaudRate" << comPort.baudRate() ;

        m_serialPort = &comPort;
        return true;
    }
    qDebug() << "Can't open port "<< portName;
    return false;
}

bool Communicaton::CloseConnection()
{
    qDebug() << __PRETTY_FUNCTION__;

    if (comPort.isOpen()){
        qDebug() << "Close port "<< comPort.portName();
        comPort.close();
        return true;
    }
    qDebug() << "Can't close port "<< comPort.portName();
    return false;
}

bool Communicaton::SendCommand(QString command)
{
    qDebug() << __PRETTY_FUNCTION__;
    qsTemp = comPort.portName();
    if (!comPort.isOpen()){
        qDebug() << "Can't send command"<<command <<"to port"<< qsTemp << ", port not open";
        return false;
    }

    //Add new line for command:
    command.append('\r');
    command.append('\n');

    m_writeData = command.toLocal8Bit();

    //QSerialPort myPort(comPort);

    //QFuture<qint64> future = QtConcurrent::run(  comPort.write, m_writeData);
    _logger->info("SendCommand");
    const qint64 bytesWritten = comPort.write(m_writeData);
    _logger->info("m_writeData : {}",m_writeData);

    if (bytesWritten == -1) {
        qDebug() << "Failed to write the data to port "<< qsTemp << "error: "<< comPort.errorString();
        return false;
    } else if (bytesWritten != m_writeData.size()) {
        qDebug() << "Failed to write all the data to port "<< qsTemp << "error: "<< comPort.errorString();
        return false;
    }

    qDebug() << "Command"<<command <<" successfully sent to port "<<qsTemp;
    return true;
}
