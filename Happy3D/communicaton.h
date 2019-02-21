#ifndef COMMUNICATON_H
#define COMMUNICATON_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QByteArray>
#include <QObject>

#include <QTextStream>
#include <QTimer>

class Communicaton : public QObject
{
    Q_OBJECT
public:
    explicit Communicaton(QObject *parent = nullptr);

private slots:
    void handleReadyRead();
    void handleTimeout();
    void handleError(QSerialPort::SerialPortError error);
    void getData();

signals:
    void readDataFromCom(QByteArray data);

private:
    QStringList m_portList;
    QSerialPort comPort;
   //QSerialPort *m_serialPort = nullptr;
    QByteArray m_writeData;
    QTextStream m_standardOutput;
    qint64 m_bytesWritten = 0;
    QTimer m_timer;

    QSerialPort *m_serialPort = nullptr;
    QByteArray m_readData;

    QString qsTemp;


public slots:
    QStringList GetInfo();
    bool OpenConnection(QString portName);
    bool CloseConnection();
    bool SendCommand(QString command);
};

#endif // COMMUNICATON_H
