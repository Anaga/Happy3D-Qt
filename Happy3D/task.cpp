#include "task.h"

Job::Job()
//default constructor
{
    this->sendTo = Enums::SendTo::toLaser;
    this-> status = Enums::CommandStatus::notInQueue;
    this->command = "";
    this->exp_res = "";
    this->delay = 0;
    this->timeout = 2000;
}

Job::Job(Enums::SendTo sendTo, QString command, int timeout)
{
    this->sendTo=sendTo;
    this->command=command;
    this->timeout = timeout;
}

Job::Job(Enums::SendTo sendTo, QString command, int delay, int timeout)
{
    this->sendTo=sendTo;
    this->command=command;
    this->delay = delay;
    this->timeout = timeout;
}

Job::Job(Enums::SendTo sendTo, QString command, QString expectedResp, int timeout)
{
    this->sendTo=sendTo;
    this->command=command;
    this->exp_res = expectedResp;
    this->timeout = timeout;
}

QString Job::print()
{
    QString retVal = "";
    QString qsStatusRow = "Job %6 status %1\n"
                  "command: %2, timeout: %3 msec,\n"
                  "exp_res: %4, delay : %5  msec";
    retVal =  qsStatusRow        .arg(Enums::print(this->status))
            .arg(this->command)  .arg(this->timeout)
            .arg(this->exp_res)  .arg(this->delay)
            .arg(Enums::print(this->sendTo));
    return retVal;
}
