#include "task.h"

Task::Task(QObject *parent) : QObject(parent)
{
   // _logger = spdlog::daily_logger_mt("Task", "logs/logfile.log", 8, 00); // new log on each morning at 8:00

    //_logger->info("Empty Task created");
    m_delay=0;
    m_status=Enums::CommandStatus::notInQueue;
    m_command="";
    m_exp_res="";
    m_timeout=2000; //2000 msec = 2sec
    m_sendTo=Enums::SendTo::notSet;
}

Task::Task(QString command, QString expRes, int delay, int timeout)
{
  //  _logger = spdlog::daily_logger_mt("Task", "logs/logfile.log", 8, 00); // new log on each morning at 8:00
  //  _logger->info("Specific Task created");
    m_delay=delay;
    m_status=Enums::CommandStatus::notInQueue;
    m_command=command;
    m_exp_res=expRes;
    m_timeout=timeout; //2000 msec = 2sec
}

Task::~Task()
{
  //  _logger->info("Task deleted");
}

QString Task::print()
{
    qDebug()  << __PRETTY_FUNCTION__ ;
    //_logger->info( __PRETTY_FUNCTION__);
    QString retVal = "";

    QString qsStatusRow = "Task %6 status %1\n"
                  "\t\t command: %2, timeout: %3 msec,\n"
                  "\t\t exp_res: %4, delay : %5  msec";
    retVal =  qsStatusRow    .arg( printStatus(m_status))
            .arg(m_command)  .arg(m_timeout)
            .arg(m_exp_res)  .arg(m_delay)
            .arg(Enums::print(m_sendTo));
    return retVal;
}

QString Task::printStatus(const Enums::CommandStatus commStat)
{
    qDebug()  << __PRETTY_FUNCTION__ ;
    switch (commStat) {
    case Enums::CommandStatus::inQueue                  : return "in Queue";
    case Enums::CommandStatus::running                  : return "running";
    case Enums::CommandStatus::notInQueue               : return "not In Queue";
    case Enums::CommandStatus::finishByTimeout          : return "finish By Timeout";
    case Enums::CommandStatus::finishByDelay            : return "finish By Delay";
    case Enums::CommandStatus::finishByCorrectResponce  : return "finish By Correct Responce";
    }
    return "CODE ERROR";
}
