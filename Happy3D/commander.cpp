#include "commander.h"
#include "commander.h"

Commander::Commander(QObject *parent) : QObject(parent)
{
    qDebug()  << __PRETTY_FUNCTION__ << "Ready!";
    timeout_timer = new QTimer(this);
    connect(timeout_timer, SIGNAL(timeout()), SLOT(commandTimeout()));

    nextTry_timer = new QTimer(this);
    connect(nextTry_timer, SIGNAL(timeout()), SLOT(nextTry()));
    nextTry_timer->start(100);
}

QStringList Commander::printTaskList()
{
    QStringList qslRet;
    Task * p_task;

    foreach (p_task, m_taskList){
        qslRet.append(p_task->print());
    }
    return qslRet;
}

QStringList Commander::printJobList()
{
    QStringList qslRet;
    foreach (auto job, m_jobList){
        qslRet.append(job.print());
    }
    //qDebug() << qslRet;
    return qslRet;
}

void Commander::runAllJob()
{
   // qDebug()  << __PRETTY_FUNCTION__  ;
    if (m_jobList.isEmpty()){
        isRunning = false;
        return;
    }
    nextTry_timer->start(100);
    if (m_jobList.first().status==Enums::CommandStatus::running){
       // qDebug() << "top job is running now";
        return;
    }
    QString const command = m_jobList.first().command;
    m_jobList.first().status = Enums::CommandStatus::running;
    qDebug() << "Running top job command " << command;

    switch (m_jobList.first().sendTo) {
    case Enums::SendTo::notSet :  qDebug() << "top command 'send to' is not define now";  break;
    case Enums::SendTo::toLaser: m_pComLaserObj->SendCommand(command); break;
    case Enums::SendTo::toPress: m_pComPressObj->SendCommand(command); break;
    }


    int const timeout = m_jobList.first().timeout;

    qDebug() << "timeout is " <<timeout << "msec";
    if (timeout){
        timeout_timer->start(timeout);
       // timeout_timer->singleShot(timeout, this, SLOT(commandTimeout()));
        qDebug() << timeout_timer->remainingTime();
    }

    int const delay =m_jobList.first().delay;

    qDebug() << "delay is " <<delay << "msec";
    if (delay){
       // QTimer::singleShot(delay, this, SLOT(delayTimeout()));
    }

}

void Commander::runTop()
{
    qDebug()  << __PRETTY_FUNCTION__  ;
    if (m_taskList.isEmpty()){
        isRunning = false;
        return;
    }
    Task *pTop = m_taskList.first();

    if (pTop->status()==Enums::CommandStatus::running){
        qDebug() << "top command is running now";
        return;
    }
    QString const command = pTop->command();
    m_taskList.first()->setStatus(Enums::CommandStatus::running);
    qDebug() << "Running top command " << command;

    switch (pTop->sendTo()) {
    case Enums::SendTo::notSet :  qDebug() << "top command 'send to' is not define now";  break;
    case Enums::SendTo::toLaser: m_pComLaserObj->SendCommand(command); break;
    case Enums::SendTo::toPress: m_pComPressObj->SendCommand(command); break;
    }


    int const timeout = pTop->timeout();

    qDebug() << "timeout is " <<timeout << "msec";
    if (timeout){
        QTimer::singleShot(timeout, this, SLOT(commandTimeout()));
    }

    int const delay = pTop->delay();

    qDebug() << "delay is " <<delay << "msec";
    if (delay){
        QTimer::singleShot(delay, this, SLOT(delayTimeout()));
    }

}

void Commander::runAll()
{
    qDebug()  << __PRETTY_FUNCTION__   << "commandList size " << m_taskList.size();
    isRunning = true;
    runTop();
}

void Commander::getResponce(QString resp)
{
    qDebug()  << __PRETTY_FUNCTION__   << "commandList size " << m_jobList.size();

    if (m_jobList.isEmpty()){
        isRunning = false;
        return;
    }
    //Task *pT = m_taskList.first();
    Job j = m_jobList.first();

    if (j.status!=Enums::CommandStatus::running){
        qDebug() << "Top commant not in running state, ignore this responce!";
        return;
    }

    if (resp.contains(j.exp_res)){
        qDebug() << "Waiting for "<<j.exp_res << " is over, got responce " <<resp;
        qDebug() << "Remove task from list, stop timer";
        timeout_timer->stop();
        m_jobList.removeFirst();
        emit taskFinished(Enums::CommandStatus::finishByCorrectResponce);
    } else {
        qDebug() << "got wrong responce";
    }

}

void Commander::delayTimeout()
{
    qDebug()  << __PRETTY_FUNCTION__   << "commandList size " << m_jobList.size();

    if (m_jobList.isEmpty()){
        isRunning = false;
        return;
    }
    //Task *pT = m_taskList.first();
    qDebug() << "Waiting is over, we got delayTimeout";
    qDebug() << "Remove task from list";
    m_jobList.removeFirst();
    emit taskFinished(Enums::CommandStatus::finishByDelay);
}

void Commander::commandTimeout()
{
    qDebug()  << __PRETTY_FUNCTION__   << "commandList size " << m_jobList.size();

    timeout_timer->stop();
    if (m_jobList.isEmpty()){
        isRunning = false;
        return;
    }

    qDebug() << "Waiting is over, we got commandTimeout";
    qDebug() << "Remove task from list";
    m_jobList.removeFirst();
    emit taskFinished(Enums::CommandStatus::finishByTimeout);
}

void Commander::nextTry()
{
    qDebug()<<__PRETTY_FUNCTION__<<"List size"<< m_jobList.size()
    << "timeout :"<<timeout_timer->remainingTime() << "msec.";
    if (m_jobList.isEmpty()){
        isRunning = false;
        nextTry_timer->stop();
        return;
    }
    runAllJob();
}
