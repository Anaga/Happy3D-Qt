#ifndef TASK_H
#define TASK_H

#include <QObject>
#include <QWidget>
#include <QString>
#include <QDebug>

#include "types.h"

struct Job {
    Enums::SendTo sendTo = Enums::SendTo::toLaser;
    Enums::CommandStatus status = Enums::CommandStatus::notInQueue;
    QString command;
    QString exp_res = "";
    int delay = 0;
    int timeout = 2000;

    QString print();
};


class Task : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString command READ command WRITE setCommand)
    Q_PROPERTY(QString exp_res READ exp_res WRITE setExp_res)
    Q_PROPERTY(int delay READ delay WRITE setDelay)
    Q_PROPERTY(int timeout READ timeout WRITE setTimeout)
    Q_PROPERTY(Enums::CommandStatus status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(Enums::SendTo sendTo READ sendTo WRITE setSendTo)

    QString m_command;
    QString m_exp_res;
    int m_delay;
    int m_timeout;
    Enums::CommandStatus m_status;
    Enums::SendTo m_sendTo;

public:
    explicit Task(QObject *parent = nullptr);
    Task(QString command="", QString expRes="",
         int delay=0, int timeout=2000);
    ~Task();
    QString print();
    QString printStatus(const Enums::CommandStatus commStat);

QString command() const
{
    return m_command;
}

QString exp_res() const
{
    return m_exp_res;
}

int delay() const
{
    return m_delay;
}

int timeout() const
{
    return m_timeout;
}

Enums::CommandStatus status() const
{
    return m_status;
}

Enums::SendTo sendTo() const
{
    return m_sendTo;
}

signals:

void statusChanged(Enums::CommandStatus status);

public slots:
void setCommand(QString command)
{
    m_command = command;
}
void setDelay(int delay)
{
    m_delay = delay;
}
void setTimeout(int timeout)
{
    m_timeout = timeout;
}
void setStatus(Enums::CommandStatus status)
{
    if (m_status == status)
        return;

    m_status = status;
    emit statusChanged(m_status);
}
void setExp_res(QString exp_res)
{
    m_exp_res = exp_res;
}
void setSendTo(Enums::SendTo sendTo)
{
    m_sendTo = sendTo;
}
};

#endif // TASK_H
