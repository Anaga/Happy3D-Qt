#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QMessageBox>
#include "communicaton.h"
#include "lasercontrol.h"



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
    void getDataFromCom(const QByteArray &arg1);

    /* UI handlers */
    void on_pushButton_Com_Refresh_clicked();
    void on_pushButton_Com_Connect_clicked();
    void on_pushButton_Com_Disconnect_clicked();    
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

    void on_pushButton_MotC__Down_clicked();

private:
    Ui::MainWindow *ui;

    Communicaton *pCommObj;
    LaserControl *pLaserObj;
};

#endif // MAINWINDOW_H
