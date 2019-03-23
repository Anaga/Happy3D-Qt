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

    Communicaton *m_pComLaserObj;
    Communicaton *m_pComPressObj;

    QLinkedList<Job> m_jobList;

    QTimer *timeout_timer;
    QTimer *nextTry_timer;
    QTimer *delay_timer;

public:
    explicit Commander(QObject *parent = nullptr);
    void setComLaser(Communicaton * pComObj) {m_pComLaserObj = pComObj;}
    void setComPress(Communicaton * pComObj) {m_pComPressObj = pComObj;}

    QStringList printJobList();

signals:
    void taskFinished(Enums::CommandStatus);

public slots:
    void addJob(Job job){
        job.status = Enums::CommandStatus::inQueue;
        m_jobList.append(job);    }

    void runAllJob();
    void getResponce(QString resp);

private slots:
    void delayTimeout();
    void commandTimeout();
    void nextTry();
};



#endif // COMMANDER_H
