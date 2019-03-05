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

#include <QtConcurrent>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/daily_file_sink.h"

class Communicaton : public QObject
{
    Q_OBJECT
public:
    explicit Communicaton(QObject *parent = nullptr);
    Communicaton(QObject *parent = nullptr, QString name = "comPort");
    ~Communicaton();

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

    std::shared_ptr<spdlog::logger> _logger;


public slots:
    QStringList GetInfo();
    bool OpenConnection(QString portName);
    bool CloseConnection();
    bool SendCommand(QString command);
};

#endif // COMMUNICATON_H
