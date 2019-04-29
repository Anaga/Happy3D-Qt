#include "commander.h"
#include "commander.h"

Commander::Commander(QObject *parent) : QObject(parent)
{
    qDebug()  << __PRETTY_FUNCTION__ << "Ready!";
    timeout_timer = new QTimer(this);
    connect(timeout_timer, SIGNAL(timeout()), SLOT(commandTimeout()));

    delay_timer = new QTimer(this);
    connect(delay_timer, SIGNAL(timeout()), SLOT(delayTimeout()));

    nextTry_timer = new QTimer(this);
    connect(nextTry_timer, SIGNAL(timeout()), SLOT(nextTry()));
    nextTry_timer->start(100);
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
        qDebug() << timeout_timer->remainingTime();
    }

    int const delay =m_jobList.first().delay;
    qDebug() << "delay is " <<delay << "msec";
    if (delay){
        delay_timer->start(delay);
    }
}


void Commander::getResponce(QString resp)
{
    qDebug()  << __PRETTY_FUNCTION__   << "commandList size " << m_jobList.size();

    if (m_jobList.isEmpty()){
        return;
    }

    Job j = m_jobList.first();

    if (j.status!=Enums::CommandStatus::running){
        qDebug() << "Top commant not in running state, ignore this responce!";
        return;
    }

    if (resp.contains(j.exp_res)){
        qDebug() << "Waiting for "<<j.exp_res << " is over, got responce " <<resp;
        qDebug() << "Remove task from list, stop timer";
        timeout_timer->stop();
        // if we got expected responce, stop waiting delay
        delay_timer->stop();
        m_jobList.removeFirst();
        emit taskFinished(Enums::CommandStatus::finishByCorrectResponce);
    } else {
        qDebug() << "got wrong responce";
    }

}

void Commander::clearJobList()
{
        qDebug()  << __PRETTY_FUNCTION__   << "commandList size " << m_jobList.size();
        delay_timer->stop();
        m_jobList.clear();
        emit taskFinished(Enums::CommandStatus::finishByCorrectResponce);
}

void Commander::delayTimeout()
{
    qDebug()  << __PRETTY_FUNCTION__   << "commandList size " << m_jobList.size();

    if (m_jobList.isEmpty()){
        delay_timer->stop();
        return;
    }

    qDebug() << "Waiting is over, we got delayTimeout";
    qDebug() << "Remove task from list";
    delay_timer->stop();
    m_jobList.removeFirst();
    emit taskFinished(Enums::CommandStatus::finishByDelay);
}

void Commander::commandTimeout()
{
    qDebug()  << __PRETTY_FUNCTION__   << "commandList size " << m_jobList.size();

    timeout_timer->stop();
    if (m_jobList.isEmpty()){
        delay_timer->stop();
        return;
    }

    qDebug() << "Waiting is over, we got commandTimeout";
    qDebug() << "Remove task from list";
    delay_timer->stop();
    m_jobList.removeFirst();
    emit taskFinished(Enums::CommandStatus::finishByTimeout);
}

void Commander::nextTry()
{
    QString qsTemp = "List size %1; ";
    qsTemp = qsTemp.arg(m_jobList.size());
    int timeoutRemTime = timeout_timer->remainingTime();
    if ( timeoutRemTime> 0){
        qsTemp.append("timeout: ");
        qsTemp.append(QString::number(timeoutRemTime));
        qsTemp.append("msec; ");
    }
    int delayRemTime = delay_timer->remainingTime();
    if ( delayRemTime> 0){
        qsTemp.append("delay: ");
        qsTemp.append(QString::number(delayRemTime));
        qsTemp.append("msec; ");
    }
    qDebug()<<__PRETTY_FUNCTION__<<  qsTemp;

    if (m_jobList.isEmpty()){
        nextTry_timer->stop();
        return;
    }
    runAllJob();
}
