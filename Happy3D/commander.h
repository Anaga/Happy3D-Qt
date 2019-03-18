#ifndef COMMANDER_H
#define COMMANDER_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <QLinkedList>
#include <QStringList>


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

public:
    explicit Commander(QObject *parent = nullptr);
    void setComLaser(Communicaton * pComObj) {m_pComLaserObj = pComObj;}
    void setComPress(Communicaton * pComObj) {m_pComPressObj = pComObj;}
    QLinkedList<Task*> taskList() const { return m_taskList;  }
    QStringList printTaskList();

signals:
    void taskFinished(Enums::CommandStatus);

public slots:
    void addTask(Task *task){ m_taskList.append(task); }
    void setTaskList(QLinkedList<Task*> taskList) { m_taskList = taskList; }
    void clearTaskList() {m_taskList.clear();}

    void runTop();
    void getResponce(QString resp);

private slots:
    void delayTimeout();
    void commandTimeout();
};



#endif // COMMANDER_H
