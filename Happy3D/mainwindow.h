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
    void on_pushButton_Com_Refresh_clicked();
    void on_pushButton_Com_Connect_clicked();

    bool inputCheck(QString text);

    void on_lineEdit_ProC_PD_textEdited(const QString &arg1);

    void on_lineEdit_ProC_ExT_textEdited(const QString &arg1);

    void on_lineEdit_Proc_LasPow_textEdited(const QString &arg1);

    void on_pushButton_Com_Disconnect_clicked();

    void getDataFromCom(const QByteArray &arg1);

    void on_pushButton_ProC_StopLaser_clicked();

    void on_pushButton_ProcC_StopMotor_clicked();

private:
    Ui::MainWindow *ui;

    Communicaton *pCommObj;
    LaserControl *pLaserObj;
};

#endif // MAINWINDOW_H
