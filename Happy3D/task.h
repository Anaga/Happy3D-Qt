#ifndef TASK_H
#define TASK_H

#include <QObject>
#include <QWidget>
#include <QString>
#include <QDebug>

#include "types.h"

struct Job {
    Job();
    Job(Enums::SendTo sendTo, QString command, int timeout = 2000);
    Job(Enums::SendTo sendTo, QString command, int delay, int timeout = 2000);
    Job(Enums::SendTo sendTo, QString command, QString expectedResp, int timeout = 2000);

    Enums::SendTo sendTo = Enums::SendTo::toLaser;
    Enums::CommandStatus status = Enums::CommandStatus::notInQueue;
    QString command;
    QString exp_res = "";
    int delay = 0;
    int timeout = 2000;

    QString print();
};

#endif // TASK_H
