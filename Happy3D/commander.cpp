#include "commander.h"
#include "commander.h"

Commander::Commander(QObject *parent) : QObject(parent)
{
    qDebug()  << __PRETTY_FUNCTION__ << "Ready!";
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

void Commander::runTop()
{
    qDebug()  << __PRETTY_FUNCTION__  ;
    if (m_taskList.isEmpty()){
        return;
    }
    Task *pTop = m_taskList.first();


    if (pTop->status()==Enums::CommandStatus::running){
        qDebug() << "top command is running now";
        return;
    }
    QString const command = pTop->command();

    qDebug() << "Running top command " << command;
    switch (pTop->sendTo()) {
    case Enums::SendTo::notSet :  qDebug() << "top command 'send to' is not define now";  break;
    case Enums::SendTo::toLaser: m_pComLaserObj->SendCommand(command); break;
    case Enums::SendTo::toPress: m_pComPressObj->SendCommand(command); break;
    }
    qDebug() << "Running top command " << command;


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

void Commander::getResponce(QString resp)
{
    qDebug()  << __PRETTY_FUNCTION__   << "commandList size " << m_taskList.size();

    if (m_taskList.isEmpty()){
        return;
    }
    Task *pT = m_taskList.first();

    if (pT->status()!=Enums::CommandStatus::running){
        qDebug() << "Top commant not in running state, ignore this responce!";
        return;
    }

    if (resp.contains(pT->exp_res())){
        qDebug() << "Waiting is over, got responce";
        qDebug() << "Remove task from list";
        m_taskList.removeFirst();

        emit taskFinished(Enums::CommandStatus::finishByCorrectResponce);
    } else {
        qDebug() << "got wrong responce";
    }

}

void Commander::delayTimeout()
{
    qDebug()  << __PRETTY_FUNCTION__   << "commandList size " << m_taskList.size();

    if (m_taskList.isEmpty()){
        return;
    }
    qDebug() << "Waiting is over, we got delayTimeout";
    qDebug() << "Remove task from list";
    m_taskList.removeFirst();
    emit taskFinished(Enums::CommandStatus::finishByDelay);
}

void Commander::commandTimeout()
{
    qDebug()  << __PRETTY_FUNCTION__   << "commandList size " << m_taskList.size();

    if (m_taskList.isEmpty()){
        return;
    }
    qDebug() << "Waiting is over, we got commandTimeout";
    qDebug() << "Remove task from list";
    m_taskList.removeFirst();
    emit taskFinished(Enums::CommandStatus::finishByTimeout);
}
