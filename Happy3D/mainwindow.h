#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QMessageBox>
#include "communicaton.h"
#include "lasercontrol.h"
#include <QElapsedTimer>
#include <QtConcurrent>
#include <QTime>


#include "spdlog/spdlog.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/basic_file_sink.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    bool inputCheck(QString text, CheckType type);
    void getDataFromPresCom(const QByteArray &arg1);
    void getDataFromLaserCom(const QByteArray &arg1);
    void motorsMove(MoveDirection dir);

    void recoaterSeq();
    void initMotors();
    
    /* UI handlers */
    void on_pushButton_Com_Refresh_clicked(); 
    void on_pushButton_ProC_StopLaser_clicked();
    void on_pushButton_ProcC_StopMotor_clicked();
    
    void on_lineEdit_ProC_PD_textEdited(const QString &arg1);
    void on_lineEdit_ProC_ExT_textEdited(const QString &arg1);
    void on_lineEdit_Proc_LasPow_textEdited(const QString &arg1);
    void on_lineEdit_Cub_LayerDist_textEdited(const QString &arg1);
    void on_lineEdit_Cub_HatchingDist_textEdited(const QString &arg1);
    void on_lineEdit_Cub_W_textEdited(const QString &arg1);
    void on_lineEdit_Cub_L_textEdited(const QString &arg1);
    void on_lineEdit_Cub_H_textEdited(const QString &arg1);
    void on_lineEdit_OxSC_OxVal_textEdited(const QString &arg1);
    void on_lineEdit_MotC_HorSpeed_textEdited(const QString &arg1);
    void on_lineEdit_MotC_HorDist_textEdited(const QString &arg1);
    void on_lineEdit_MotC_VertSpeed_textEdited(const QString &arg1);
    void on_lineEdit_MotC_VertDist_textEdited(const QString &arg1);
    
    void on_pushButton_MotC_Left_clicked();
    void on_pushButton_MotC_Rigth_clicked();
    void on_pushButton_MotC_Up_clicked();
    void on_pushButton_MotC_Down_clicked();
    
    void on_pushButton_Com_Las_OC_clicked(bool checked);
    void on_pushButton_Com_Pres_OC_clicked(bool checked);
    
    void on_pushButton_OxSC_Start_clicked();
    void on_pushButton__OxSC_Stop_clicked();
    
    void on_pushButton_GasCp_On_clicked();
    void on_pushButton_GasCp_Of_clicked();
    
    void on_pushButton_RecC_LeftUp_clicked();
    void on_pushButton_RecC_LeftDown_clicked();
    void on_pushButton_RecC_RigthUp_clicked();
    void on_pushButton_RecC_RifgthDown_clicked();
    
    void on_pushButton_PushC_Pull_clicked();
    void on_pushButton_PushC_TPull_clicked();
    void on_pushButton_PushC_Hold_clicked();
    void on_pushButton_PushC_TPush_clicked();
    void on_pushButton_PushC_Push_clicked();
    
    void on_pushButton_RecC_Sec_clicked();
    
    void Delay_MSec(unsigned int msec);
    
    void on_pushButton_Init_MX_clicked();
    
    void on_pushButton_Init_MY_clicked();

private:
    Ui::MainWindow *ui;

    Communicaton *pComLaserObj;
    Communicaton *pComPresObj;
    
    LaserControl *pLaserObj;
    QElapsedTimer t;
    QElapsedTimer globalTimer;
    
    QString command;
    QString qsTemp;
    
    std::shared_ptr<spdlog::logger> _logger;

};

#endif // MAINWINDOW_H
