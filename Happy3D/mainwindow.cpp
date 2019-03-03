#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _logger = spdlog::daily_logger_mt("MainWindows", "logs/logfile.log", 8, 00); // new log on each morning at 8:00

    _logger->info("MainWindow startup");

    pComLaserObj = new Communicaton();
    pComPresObj = new Communicaton();
    on_pushButton_Com_Refresh_clicked();

    connect( pComLaserObj, SIGNAL(readDataFromCom(QByteArray)), this, SLOT(getDataFromLaserCom(const QByteArray)));
    connect( pComPresObj,  SIGNAL(readDataFromCom(QByteArray)), this, SLOT(getDataFromPresCom(const QByteArray)));
    pLaserObj = new LaserControl;

}

MainWindow::~MainWindow()
{
    _logger->info("MainWindow shutdown");
    delete ui;
}

void MainWindow::on_pushButton_Com_Refresh_clicked()
{
    ui->comboBox_Com_Laser_Select->clear();
    ui->comboBox_Com_Press_Select->clear();
    QStringList portList;
    pComPresObj->GetInfo();
    portList = pComLaserObj->GetInfo();

    if (portList.size()>0) {
        for (auto port :  portList) {
            ui->comboBox_Com_Laser_Select->addItem(port);
            ui->comboBox_Com_Press_Select->addItem(port);
        }
    }
}


bool MainWindow::inputCheck(QString text, CheckType type)
{
    /* Warning:
     *
     * The QString content may only contain valid numerical characters
     * which includes the plus/minus sign, the characters g and e used in scientific notation,
     * and the decimal point.
     *
     * Including the unit or additional characters leads to a conversion error.
     */
    bool bOk = false;
    qreal fVal = 0.0;
    long iVal = 0;
    QString qsTemp = "Can't convert %2 to %1!";

    if (type == Integer){
        iVal = text.toLong(&bOk);
        qsTemp = qsTemp.arg("Integer");
    }
    if (type == Double){
        fVal = text.toDouble(&bOk);
        qsTemp = qsTemp.arg("Double");
    }

    if (!bOk){
        qsTemp = qsTemp.arg(text);
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(qsTemp);
        msgBox.exec();
    }
    qDebug() << "iVal is " << iVal;
    qDebug() << "fVal is " << fVal;
    return bOk;
}


void MainWindow::on_lineEdit_ProC_PD_textEdited(const QString &arg1)
{
    // #step=N – point distance in µm (float)
    if (!inputCheck(arg1,Double)) {
        ui->lineEdit_ProC_PD->clear();
    }
}

void MainWindow::on_lineEdit_ProC_ExT_textEdited(const QString &arg1)
{
    // #del=N – exposure time in µs (integer)
    if (!inputCheck(arg1, Integer)) {
        ui->lineEdit_ProC_ExT->clear();
    }
}

void MainWindow::on_lineEdit_Proc_LasPow_textEdited(const QString &arg1)
{
    //#pow=N – laser power in percent (float) (minimum laser power with visible beam is 3.87%)
    if (!inputCheck(arg1,Double)) {
        ui->lineEdit_Proc_LasPow->clear();
    }
}

void MainWindow::getDataFromPresCom(const QByteArray &arg1)
{
    // qDebug() << __PRETTY_FUNCTION__;
    QString qsTemp(arg1);
    qsTemp = qsTemp.trimmed();    
    static double lastOxValue = 0;

    if (qsTemp.contains("Oxygen value:")){
        qsTemp= qsTemp.replace("\r\n", ":");
        QStringList qslOx = qsTemp.split(":");
        double oxVal = qslOx.at(1).toDouble();

        // do not logging Oxygen value, only on change
        if ( !qFuzzyCompare(lastOxValue, oxVal)) {  // function for floating points comparrison
            ui->lineEdit_OxSC_OxVal->setText(QString::number(oxVal));
            lastOxValue = oxVal;
            ui->textBrowser_log->append(qsTemp);
            _logger->info("Data from press: {}",qPrintable(qsTemp));
        }
        return;        
    }
    ui->textBrowser_log->append(qsTemp);
    _logger->info("Data from press: {}",qPrintable(qsTemp));
}

void MainWindow::getDataFromLaserCom(const QByteArray &arg1)
{
    // qDebug() << __PRETTY_FUNCTION__;
    QString qsTemp(arg1);
    qsTemp = qsTemp.trimmed();
    ui->textBrowser_log->append(qsTemp);
    _logger->info("Data from laser: {}",qPrintable(qsTemp));
    //ui->textBrowser_Main_Trans->append(qsTemp);
}

void MainWindow::motorsMove(MoveDirection dir)
{
    long speed =0;
    long dist =0;
    QString command;
    if ((dir==Left) || (dir==Right)) {
        speed = ui->lineEdit_MotC_HorSpeed->text().toLong();
        dist =  ui->lineEdit_MotC_HorDist->text().toLong();
    }

    if ((dir==Up) || (dir==Down)) {
        speed = ui->lineEdit_MotC_VertSpeed->text().toLong();
        dist =  ui->lineEdit_MotC_VertDist->text().toLong();
    }
    command = pLaserObj->moveMotors(dir,dist,speed);
    qDebug() << "We will send to laser this row:" << command;
    pComLaserObj->SendCommand(command);
}

void MainWindow::initMotors()
{
    _logger->info("initMotors");
    QString command;
    qint64 timeout = 1000; //milliseconds
    globalTimer.start();
    while(globalTimer.elapsed()<timeout);
    command = pLaserObj->initMotors(X);
    qDebug() << "We will send to laser this row:" << command;
    _logger->info("We will send to laser this row: {}", qPrintable(command));
    pComLaserObj->SendCommand(command);

    globalTimer.start();
    while(globalTimer.elapsed()<timeout);
    command = pLaserObj->initMotors(X);
    qDebug() << "We will send to laser this row:" << command;
    _logger->info("We will send to laser this row: {}", qPrintable(command));
    pComLaserObj->SendCommand(command);

}

void MainWindow::recoaterSeq()
{
    /*
            SerialConnection.SendData1("R");//Right up
            Delay(500);
            SerialConnection.SendData1("F"); //left down
            Delay(500);
            MoveMotor2("13000", "y", "1");//Motor2 run to the right end
            DelayS(9);
            SerialConnection.SendData1("I");//Right down
            Delay(500);
            SerialConnection.SendData1("L"); //left up
            Delay(500);
            MoveMotor2("8300", "y", "2");//Motor2 run to the push position
            DelayS(6);
            SerialConnection.SendData1("D");//Push
            Delay(500);
            MoveMotor1(Motor1MoveText.Text, "x", "1");//Motor1 down one layer
            Delay(500);
            SerialConnection.SendData1("U");//Pull
            Delay(900);
            SerialConnection.SendData1("H");//Hold
            Delay(500);
            MoveMotor2("4700", "y", "2");
            DelayS(4);
            SerialConnection.SendData1("F"); //left down
      */
}

void MainWindow::on_pushButton_ProC_StopLaser_clicked()
{
    QString qsTemp = pLaserObj->stopLaser();
    qDebug() << "We will send to laser this row:" << qsTemp;
    pComLaserObj->SendCommand(qsTemp);
}

void MainWindow::on_pushButton_ProcC_StopMotor_clicked()
{
    QString qsTemp = pLaserObj->stopMotor();
    qDebug() << "We will send to laser this row:" << qsTemp;
    pComLaserObj->SendCommand(qsTemp);
}

void MainWindow::on_lineEdit_Cub_LayerDist_textEdited(const QString &arg1)
{
    if (!inputCheck(arg1, Integer)) {
        ui->lineEdit_Cub_LayerDist->clear();
    }
}

void MainWindow::on_lineEdit_Cub_HatchingDist_textEdited(const QString &arg1)
{
    if (!inputCheck(arg1,Double)) {
        ui->lineEdit_Cub_HatchingDist->clear();
    }
}

void MainWindow::on_lineEdit_Cub_W_textEdited(const QString &arg1)
{
    if (!inputCheck(arg1, Integer)) {
        ui->lineEdit_Cub_W->clear();
    }
}

void MainWindow::on_lineEdit_Cub_L_textEdited(const QString &arg1)
{
    if (!inputCheck(arg1, Integer)) {
        ui->lineEdit_Cub_L->clear();
    }
}

void MainWindow::on_lineEdit_Cub_H_textEdited(const QString &arg1)
{
    if (!inputCheck(arg1, Integer)) {
        ui->lineEdit_Cub_H->clear();
    }
}

void MainWindow::on_lineEdit_OxSC_OxVal_textEdited(const QString &arg1)
{
    if (!inputCheck(arg1,Double)) {
        ui->lineEdit_OxSC_OxVal->clear();
    }
}

void MainWindow::on_lineEdit_MotC_HorSpeed_textEdited(const QString &arg1)
{
    //speed – 0 – 12500, in µm/s
    if (!inputCheck(arg1, Integer)) {
        ui->lineEdit_MotC_HorSpeed->clear();
    }
}

void MainWindow::on_lineEdit_MotC_HorDist_textEdited(const QString &arg1)
{
    //distance – any integer value in um
    if (!inputCheck(arg1, Integer)) {
        ui->lineEdit_MotC_HorDist->clear();
    }
}

void MainWindow::on_lineEdit_MotC_VertSpeed_textEdited(const QString &arg1)
{
    //speed – 0 – 12500, in µm/s
    if (!inputCheck(arg1, Integer)) {
        ui->lineEdit_MotC_VertSpeed->clear();
    }
}

void MainWindow::on_lineEdit_MotC_VertDist_textEdited(const QString &arg1)
{
    //distance – any integer value in um
    if (!inputCheck(arg1, Integer)) {
        ui->lineEdit_MotC_VertDist->clear();
    }
}

void MainWindow::on_pushButton_MotC_Left_clicked()
{
    motorsMove(Left);
}

void MainWindow::on_pushButton_MotC_Rigth_clicked()
{
    motorsMove(Right);
}

void MainWindow::on_pushButton_MotC_Up_clicked()
{
    motorsMove(Up);
}

void MainWindow::on_pushButton_MotC_Down_clicked()
{
    motorsMove(Down);
}

void MainWindow::on_pushButton_Com_Las_OC_clicked(bool checked)
{
    QString qsPortName = ui->comboBox_Com_Laser_Select->currentText();
    QString qsTemp = "Laser port %1";

    if (checked){
        if (pComLaserObj->OpenConnection(qsPortName)){
            qsTemp = qsTemp.arg("is open");
            ui->pushButton_Com_Las_OC->setText("Close");
            initMotors();
            /*
            t.start();
            while(t.elapsed()<1000);
            pComLaserObj->SendCommand("#mx=S,200\r\n");
            t.start();
            while(t.elapsed()<1000);
            pComLaserObj->SendCommand("#my=S,200\r\n");
            */
        } else {
            qsTemp = qsTemp.arg("not open");
        }
    } else {
        if (pComLaserObj->CloseConnection()){
            qsTemp = qsTemp.arg("is closed");
            ui->pushButton_Com_Las_OC->setText("Open");
        } else {
            qsTemp = qsTemp.arg("not closed");
        }
    }
    ui->label_Com_Las_Status->setText(qsTemp);
}

void MainWindow::on_pushButton_Com_Pres_OC_clicked(bool checked)
{
    QString qsPortName = ui->comboBox_Com_Press_Select->currentText();
    QString qsTemp = "Press port %1";

    if (checked){
        if (pComPresObj->OpenConnection(qsPortName)){
            qsTemp = qsTemp.arg("is open");
            ui->pushButton_Com_Pres_OC->setText("Close");
        } else {
            qsTemp = qsTemp.arg("not open");
        }
    } else {
        if (pComPresObj->CloseConnection()){
            qsTemp = qsTemp.arg("is closed");
            ui->pushButton_Com_Pres_OC->setText("Open");
        } else {
            qsTemp = qsTemp.arg("not closed");
        }
    }
    ui->label_Com_Pres_Status->setText(qsTemp);
}

void MainWindow::on_pushButton_OxSC_Start_clicked()
{
    pComPresObj->SendCommand("serial start");
}

void MainWindow::on_pushButton__OxSC_Stop_clicked()
{
    pComPresObj->SendCommand("serial stop");
}

void MainWindow::on_pushButton_GasCp_On_clicked()
{
    //pComPresObj->SendCommand("M");
}

void MainWindow::on_pushButton_GasCp_Of_clicked()
{
    //pComPresObj->SendCommand("N");
}

void MainWindow::on_pushButton_RecC_LeftUp_clicked()
{
    pComPresObj->SendCommand("L");
}

void MainWindow::on_pushButton_RecC_LeftDown_clicked()
{
    pComPresObj->SendCommand("F");//left down
}

void MainWindow::on_pushButton_RecC_RigthUp_clicked()
{
    pComPresObj->SendCommand("R");//Right up
}

void MainWindow::on_pushButton_RecC_RifgthDown_clicked()
{
    pComPresObj->SendCommand("I");//Right down
}

void MainWindow::on_pushButton_PushC_Pull_clicked()
{
     pComPresObj->SendCommand("U");//Pull
}

void MainWindow::on_pushButton_PushC_TPull_clicked()
{
    pComPresObj->SendCommand("U");//Pull
}

void MainWindow::on_pushButton_PushC_Hold_clicked()
{
    pComPresObj->SendCommand("H");//Hold
}

void MainWindow::on_pushButton_PushC_TPush_clicked()
{
    pComPresObj->SendCommand("D");//Push
}

void MainWindow::on_pushButton_PushC_Push_clicked()
{
    pComPresObj->SendCommand("D");//Push
}
