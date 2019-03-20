#ifndef COMMANDER_H
#define COMMANDER_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <QLinkedList>
#include <QStringList>
#include <QTimer>



#include "types.h"
#include "task.h"
#include "communicaton.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/daily_file_sink.h"

class Commander : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QLinkedList<Task*> taskList READ taskList WRITE setTaskList)

    Communicaton *m_pComLaserObj;
    Communicaton *m_pComPressObj;
    QLinkedList<Task*> m_taskList;
    QLinkedList<Job> m_jobList;
    bool isRunning = false;
    //QTimer timeout;
    QTimer *timeout_timer;
    QTimer *nextTry_timer;

public:
    explicit Commander(QObject *parent = nullptr);
    void setComLaser(Communicaton * pComObj) {m_pComLaserObj = pComObj;}
    void setComPress(Communicaton * pComObj) {m_pComPressObj = pComObj;}
    QLinkedList<Task*> taskList() const { return m_taskList;  }
    QStringList printTaskList();
    QStringList printJobList();

signals:
    void taskFinished(Enums::CommandStatus);

public slots:
    void addJob(Job job){
        job.status = Enums::CommandStatus::inQueue;
        m_jobList.append(job);    }

    void runAllJob();

    void addTask(Task *task){
        task->setStatus(Enums::CommandStatus::inQueue);
        m_taskList.append(task); }
    void setTaskList(QLinkedList<Task*> taskList) { m_taskList = taskList; }
    void clearTaskList() {m_taskList.clear();}

    void runTop();
    void runAll();
    void getResponce(QString resp);

private slots:
    void delayTimeout();
    void commandTimeout();
    void nextTry();
};



#endif // COMMANDER_H
