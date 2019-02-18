#ifndef COMMUNICATON_H
#define COMMUNICATON_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>
#include <QStringList>
#include <QDebug>

class Communicaton : public QObject
{
    Q_OBJECT
public:
    explicit Communicaton(QObject *parent = nullptr);

signals:

private:
    QStringList qslRet;

public slots:
    QStringList GetInfo();
};

#endif // COMMUNICATON_H
