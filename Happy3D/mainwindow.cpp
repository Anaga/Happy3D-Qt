#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _logger = spdlog::daily_logger_mt("MainWindows", "logs/logfile.log", 8, 00); // new log on each morning at 8:00

    _logger->info("MainWindow startup");

    pComLaserObj = new Communicaton(this, "Laser");
    pComPresObj = new Communicaton(this, "Press");
    on_pushButton_Com_Refresh_clicked();

    connect( pComLaserObj, SIGNAL(readDataFromCom(QByteArray)), this, SLOT(getDataFromLaserCom(const QByteArray)));
    connect( pComPresObj,  SIGNAL(readDataFromCom(QByteArray)), this, SLOT(getDataFromPresCom(const QByteArray)));
    pLaserObj = new LaserControl;
    command = "";

    pGeneral = new Commander();
    pGeneral->setComLaser(pComLaserObj);
    pGeneral->setComPress(pComPresObj);

    connect( pGeneral, SIGNAL(taskFinished(Enums::CommandStatus)),
             this,  SLOT(handleJobFinished(Enums::CommandStatus)));
    handleJobFinished(Enums::CommandStatus::notInQueue);
}

MainWindow::~MainWindow()
{
    _logger->info("MainWindow shutdown");
    delete ui;
}

QList<Job> MainWindow::sqere(float size)
{
    double CentX = 5.0;
    double CentY = 5.0;

    QPointF leftTop;
//# Fix me
    leftTop.setX( (qreal)(CentX-(size/2.0)));
    leftTop.setY( (qreal)(CentY-(size/2.0)));
    qDebug() << "leftTop" << leftTop;

    QPointF rigthTop;
    rigthTop.setX( (qreal)(CentX+(size/2.0)));
    rigthTop.setY( (qreal)(CentY-(size/2.0)));
    qDebug() << "rigthTop" << rigthTop;

    QPointF rigthBottom;
    rigthBottom.setX( (qreal)(CentX+(size/2.0)));
    rigthBottom.setY( (qreal)(CentY+(size/2.0)));
    qDebug() << "rigthBottom" << rigthBottom;

    QPointF leftBottom;
    leftBottom.setX( (qreal)(CentX-(size/2.0)));
    leftBottom.setY( (qreal)(CentY+(size/2.0)));
    qDebug() << "leftBottom" << leftBottom;

    QString qsTemp = "#line=%1,%2,%3,%4";
    Job topLine;
    topLine.sendTo = Enums::SendTo::toLaser;
    topLine.timeout = 1000* static_cast<int>(size)+1000;
    topLine.command = qsTemp.arg(leftTop.x()).arg(leftTop.y())
            .arg(rigthTop.x()).arg(rigthTop.y());
    qDebug() << "topLine command " << topLine.command ;
    topLine.exp_res = "LINE_OK";

    Job topLine1(topLine);
    topLine1.command = qsTemp.arg(leftTop.x()).arg(leftTop.y()+0.2)
            .arg(rigthTop.x()).arg(rigthTop.y()+0.2);

    Job topLine2(topLine);
    topLine2.command = qsTemp.arg(leftTop.x()).arg(leftTop.y()-0.2)
            .arg(rigthTop.x()).arg(rigthTop.y()-0.2);

    Job leftLine(topLine);
    leftLine.command = qsTemp.arg(leftTop.x()).arg(leftTop.y())
            .arg(leftBottom.x()).arg(leftBottom.y());
    qDebug() << "LeftLine command " << leftLine.command ;

    Job bottomLine(topLine);
    bottomLine.command = qsTemp.arg(rigthBottom.x()).arg(rigthBottom.y())
            .arg(leftBottom.x()).arg(leftBottom.y());
    qDebug() << "bottomLine.command " << bottomLine.command ;

    Job rigthLine(topLine);
    rigthLine.command = qsTemp.arg(rigthTop.x()).arg(rigthTop.y())
            .arg(rigthBottom.x()).arg(rigthBottom.y());
    qDebug() << "rigthLine .command " << rigthLine.command ;

    QList<Job> retList;
    retList << topLine << topLine1 << topLine2 << leftLine << bottomLine << rigthLine;
    return  retList;

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


bool MainWindow::inputCheck(QString text, Enums::CheckType type)
{
    /* Warning:
     *
     * The QString content may only contain valid numerical characters
     * which includes the plus/minus sign, the characters g and e used in scientific notation,
     * and the decimal point.
     *
     * Including the unit or additional characters leads to a conversion error.
     */
    if (text.isEmpty()) return false;
    bool bOk = false;
    qreal fVal = 0.0;
    long iVal = 0;
    qsTemp = "Can't convert %2 to %1!";

    if (type == Enums::Integer){
        iVal = text.toLong(&bOk);
        qsTemp = qsTemp.arg("Integer");
    }
    if (type == Enums::Double){
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
    if (!inputCheck(arg1,Enums::Double)) {
        ui->lineEdit_ProC_PD->clear();
    }
}

void MainWindow::on_lineEdit_ProC_ExT_textEdited(const QString &arg1)
{
    // #del=N – exposure time in µs (integer)
    if (!inputCheck(arg1, Enums::Integer)) {
        ui->lineEdit_ProC_ExT->clear();
    }
}

void MainWindow::on_lineEdit_Proc_LasPow_textEdited(const QString &arg1)
{
    //#pow=N – laser power in percent (float) (minimum laser power with visible beam is 3.87%)
    if (!inputCheck(arg1,Enums::Double)) {
        ui->lineEdit_Proc_LasPow->clear();
    }
}

void MainWindow::getDataFromPresCom(const QByteArray &arg1)
{
    qsTemp = arg1.trimmed();
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
    pGeneral->getResponce(qsTemp);
    ui->textBrowser_log->append(qsTemp);
    _logger->info("Data from press: {}",qPrintable(qsTemp));
}

void MainWindow::getDataFromLaserCom(const QByteArray &arg1)
{
    // qDebug() << __PRETTY_FUNCTION__;
    QString qsTemp(arg1);
    qsTemp = qsTemp.trimmed();
    ui->textBrowser_log->append(qsTemp);
    pGeneral->getResponce(qsTemp);
    _logger->info("Data from laser: {}",qPrintable(qsTemp));
    //ui->textBrowser_Main_Trans->append(qsTemp);
}

void MainWindow::motorsMove(Enums::MoveDirection dir)
{
    long speed =0;
    long dist =0;
    if ((dir==Enums::Left) || (dir==Enums::Right)) {
        speed = ui->lineEdit_MotC_HorSpeed->text().toLong();
        dist =  ui->lineEdit_MotC_HorDist->text().toLong();
    }

    if ((dir==Enums::Up) || (dir==Enums::Down)) {
        speed = ui->lineEdit_MotC_VertSpeed->text().toLong();
        dist =  ui->lineEdit_MotC_VertDist->text().toLong();
    }
    command = pLaserObj->moveMotors(dir,dist,speed);
    qDebug() << "We will send to laser this row:" << command;
    pComLaserObj->SendCommand(command);
}

void MainWindow::handleJobFinished(const Enums::CommandStatus status)
{
    QPixmap pxGren = QPixmap(":/img/LED-GRN-small.png");
    QPixmap pxRed = QPixmap(":/img/LED-RED-small.png");
    QPixmap pxWhite = QPixmap(":/img/LED-WHT-small.png");
    QPixmap pxYellow = QPixmap(":/img/LED-YEL-small.png");
    switch (status) {
    case Enums::CommandStatus::finishByCorrectResponce :ui->label_status->setPixmap(pxGren); break;
    case Enums::CommandStatus::finishByTimeout :ui->label_status->setPixmap(pxRed); break;
    case Enums::CommandStatus::finishByDelay :ui->label_status->setPixmap(pxYellow); break;
    default: ui->label_status->setPixmap(pxWhite);
    }
    qDebug() << Enums::print(status);
    updateJobList(pGeneral->printJobList());
}

void MainWindow::updateJobList(const QStringList jobList)
{
    ui->plainTextEdit_JobList->clear();

    ui->plainTextEdit_JobList->appendPlainText("updateJobList");
    for (auto jobDisk : jobList){
        ui->plainTextEdit_JobList->appendPlainText(jobDisk);
    }
    //ui->plainTextEdit_JobList->moveCursor();
}

void MainWindow::initMotors()
{
    _logger->info("initMotors");
    Job init(Enums::SendTo::toLaser, "#mx=S,200", "$MX:S,200");
    pGeneral->addJob(init);

    init.command = "#my=S,200";
    init.exp_res = "$MY:S,200";
    pGeneral->addJob(init);

    QStringList jobs = pGeneral->printJobList();
    updateJobList(jobs);
    pGeneral->runAllJob();

    ui->pushButton_ProC_SentLaserSettings->click();

}

void MainWindow::recoaterSeq()
{
    _logger->info(__PRETTY_FUNCTION__);

    int iDelay = 500; //milliseconds
    int iTimeout = 1000; //msec
    Job rocControl(Enums::SendTo::toPress, "R", iDelay, iTimeout);
    pGeneral->addJob(rocControl);

    rocControl.command="F";
    pGeneral->addJob(rocControl);

    //Motor2 run to the right end
    long distance = 13000;
    long speed = 1600;
    Job motControl(Enums::SendTo::toLaser, "", iDelay, iTimeout);
    motControl.command = pLaserObj->moveMotors(Enums::Right, distance, speed);
    motControl.delay=9000;
    // timeout shall be more than delay
    motControl.timeout = 9000 + iDelay;
    pGeneral->addJob(motControl);
    qDebug() << "We will send to laser this row:" << motControl.command;

    rocControl.command="I";
    pGeneral->addJob(rocControl);

    rocControl.command="L";
    pGeneral->addJob(rocControl);

    pGeneral->runAllJob();

    //Motor2 run to the push position
    // MoveMotor2("8300", "y", "2");
    /*distance = 8300;
    motControl.command = pLaserObj->moveMotors(Enums::Left, distance, speed);
    motControl.delay=6000;
    motControl.timeout = motControl.delay + iDelay;
    pGeneral->addJob(motControl);
    qDebug() << "We will send to laser this row:" << motControl.command;

    rocControl.command="D";
    pGeneral->addJob(rocControl);

    //Motor1 down one layer
    // MoveMotor1(Motor1MoveText.Text, "x", "1");
    qsTemp = ui->lineEdit_MotC_VertDist->text();
    distance = qsTemp.toLong();
    motControl.command = pLaserObj->moveMotors(Enums::Down, distance, speed);
    pGeneral->addJob(motControl);
    qDebug() << "We will send to laser this row:" << motControl.command;

    rocControl.command="U";
    pGeneral->addJob(rocControl);

    rocControl.command="H";
    pGeneral->addJob(rocControl);

    //MoveMotor2("4700", "y", "2");
    distance = 4700;
    motControl.command = pLaserObj->moveMotors(Enums::Left, distance, speed);
    motControl.delay=4000;
    motControl.timeout = motControl.delay + iDelay;
    pGeneral->addJob(motControl);
    qDebug() << "We will send to laser this row:" << motControl.command;
    //pComLaserObj->SendCommand(command);
    //Delay_MSec(4000);

    // //left down
    rocControl.command="F";
    pGeneral->addJob(rocControl);

    QStringList jobs = pGeneral->printJobList();
    updateJobList(jobs);
    qDebug() << "Now we start to run all jobs! ";
    pGeneral->runAllJob();*/

    /*qint64 timeout = 500; //milliseconds

    pComPresObj->SendCommand("R"); //Right up
    globalTimer.start();
    while(globalTimer.elapsed()<timeout);

    pComPresObj->SendCommand("F"); //left down
    globalTimer.start();
    while(globalTimer.elapsed()<timeout);


    //Motor2 run to the right end
    long distance = 13000;
    long speed = 1600;
    command = pLaserObj->moveMotors(Right, distance, speed);
    qDebug() << "We will send to laser this row:" << command;
    pComLaserObj->SendCommand(command);
    timeout = 9000; // 9000 milliseconds = 9 sec
    globalTimer.start();
    while(globalTimer.elapsed()<timeout);

    timeout = 500;
    pComPresObj->SendCommand("I"); //Right down
    globalTimer.start();
    while(globalTimer.elapsed()<timeout);

    pComPresObj->SendCommand("L"); //left down
    globalTimer.start();
    while(globalTimer.elapsed()<timeout);

    //Motor2 run to the push position
    // MoveMotor2("8300", "y", "2");
    distance = 8300;
    command = pLaserObj->moveMotors(Right, distance, speed);
    qDebug() << "We will send to laser this row:" << command;
    pComLaserObj->SendCommand(command);
    timeout = 6000; // 6000 milliseconds = 6 sec
    globalTimer.start();
    while(globalTimer.elapsed()<timeout);

    timeout = 500;
    pComPresObj->SendCommand("D"); //Push
    globalTimer.start();
    while(globalTimer.elapsed()<timeout);



    //Motor1 down one layer
    // MoveMotor1(Motor1MoveText.Text, "x", "1");
    qsTemp = ui->lineEdit_MotC_VertDist->text();
    distance = qsTemp.toLong();
    command = pLaserObj->moveMotors(Down, distance, speed);
    qDebug() << "We will send to laser this row:" << command;
    pComLaserObj->SendCommand(command);
    timeout = 500; // 6000 milliseconds = 6 sec
    globalTimer.start();
    while(globalTimer.elapsed()<timeout);

    timeout = 900;
    pComPresObj->SendCommand("U"); //Pull
    globalTimer.start();
    while(globalTimer.elapsed()<timeout);

    timeout = 500;
    pComPresObj->SendCommand("H"); //Hold
    globalTimer.start();
    while(globalTimer.elapsed()<timeout);

    //MoveMotor2("4700", "y", "2");
    distance = 4700;
    command = pLaserObj->moveMotors(Left, distance, speed);
    qDebug() << "We will send to laser this row:" << command;
    pComLaserObj->SendCommand(command);
    timeout = 4000;
    globalTimer.start();
    while(globalTimer.elapsed()<timeout);

    pComPresObj->SendCommand("F"); //left down


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

    pComPresObj->SendCommand("R");//Right up
        Delay_MSec(500);
        pComPresObj->SendCommand("F"); //left down
        Delay_MSec(500);
        pComLaserObj->SendCommand("#my=1,13000,1600\r\n");//Motor2 run to the right end
        Delay_MSec(9000);
        pComPresObj->SendCommand("I");//Right down
        Delay_MSec(500);
        pComPresObj->SendCommand("L"); //left up
        Delay_MSec(500);
        pComLaserObj->SendCommand("#my=2,8300,1600\r\n");//Motor2 run to the push position
        Delay_MSec(6000);
        pComPresObj->SendCommand("D");//Push
        Delay_MSec(500);
        motorsMove(Down);//Motor1 down one layer
        Delay_MSec(500);
        pComPresObj->SendCommand("U");//Pull
        Delay_MSec(500);
        pComPresObj->SendCommand("H");//Hold
        Delay_MSec(500);
        pComLaserObj->SendCommand("#my=2,4700,1600\r\n");
        Delay_MSec(4000);
        pComPresObj->SendCommand("F"); //left down

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
    if (!inputCheck(arg1, Enums::Integer)) {
        ui->lineEdit_Cub_LayerDist->clear();
    }
}

void MainWindow::on_lineEdit_Cub_HatchingDist_textEdited(const QString &arg1)
{
    if (!inputCheck(arg1,Enums::Double)) {
        ui->lineEdit_Cub_HatchingDist->clear();
    }
}

void MainWindow::on_lineEdit_Cub_W_textEdited(const QString &arg1)
{
    if (!inputCheck(arg1, Enums::Integer)) {
        ui->lineEdit_Cub_W->clear();
    }
}

void MainWindow::on_lineEdit_Cub_L_textEdited(const QString &arg1)
{
    if (!inputCheck(arg1, Enums::Integer)) {
        ui->lineEdit_Cub_L->clear();
    }
}

void MainWindow::on_lineEdit_Cub_H_textEdited(const QString &arg1)
{
    if (!inputCheck(arg1, Enums::Integer)) {
        ui->lineEdit_Cub_H->clear();
    }
}

void MainWindow::on_lineEdit_OxSC_OxVal_textEdited(const QString &arg1)
{
    if (!inputCheck(arg1, Enums::Double)) {
        ui->lineEdit_OxSC_OxVal->clear();
    }
}

void MainWindow::on_lineEdit_MotC_HorSpeed_textEdited(const QString &arg1)
{
    //speed – 0 – 12500, in µm/s
    if (!inputCheck(arg1, Enums::Integer)) {
        ui->lineEdit_MotC_HorSpeed->clear();
    }
}

void MainWindow::on_lineEdit_MotC_HorDist_textEdited(const QString &arg1)
{
    //distance – any integer value in um
    if (!inputCheck(arg1, Enums::Integer)) {
        ui->lineEdit_MotC_HorDist->clear();
    }
}

void MainWindow::on_lineEdit_MotC_VertSpeed_textEdited(const QString &arg1)
{
    //speed – 0 – 12500, in µm/s
    if (!inputCheck(arg1, Enums::Integer)) {
        ui->lineEdit_MotC_VertSpeed->clear();
    }
}

void MainWindow::on_lineEdit_MotC_VertDist_textEdited(const QString &arg1)
{
    //distance – any integer value in um
    if (!inputCheck(arg1, Enums::Integer)) {
        ui->lineEdit_MotC_VertDist->clear();
    }
}

void MainWindow::on_pushButton_MotC_Left_clicked()
{
    motorsMove(Enums::Left);
}

void MainWindow::on_pushButton_MotC_Rigth_clicked()
{
    motorsMove(Enums::Right);
}

void MainWindow::on_pushButton_MotC_Up_clicked()
{
    motorsMove(Enums::Up);
}

void MainWindow::on_pushButton_MotC_Down_clicked()
{
    motorsMove(Enums::Down);
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

void MainWindow::on_pushButton_RecC_Sec_clicked()
{
    recoaterSeq();
}
void MainWindow::Delay_MSec(unsigned int msec)
{
    QTime _Timer = QTime::currentTime().addMSecs(static_cast<int>(msec));

    while( QTime::currentTime() < _Timer )

        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
/*
void MainWindow::on_pushButton_initMotors_clicked()
{

    qint64 timeout = 5000; //milliseconds
    globalTimer.start();
    while(globalTimer.elapsed()<timeout);

    command = pLaserObj->initMotors(X);
    qDebug() << "We will send to laser this row:" << command;
    _logger->info("We will send to laser this row: {}", qPrintable(command));
    pComLaserObj->SendCommand(command);


    globalTimer.start();
    while(globalTimer.elapsed()<timeout);
    command = pLaserObj->initMotors(Y);
    qDebug() << "We will send to laser this row:" << command;
    _logger->info("We will send to laser this row: {}", qPrintable(command));
    pComLaserObj->SendCommand(command);

}
 */
void MainWindow::on_pushButton_Init_MX_clicked()
{
    Job initX;
    initX.sendTo = Enums::SendTo::toLaser;
    initX.command = pLaserObj->initMotors(Enums::X);
    initX.exp_res = "$MX:S,200";
    // we send short command to laser, and waiting resp


    pGeneral->addJob(initX);
    qDebug() << "We will send to laser this row:" << initX.command;
    _logger->info("We will send to laser this row: {}", qPrintable(initX.command));
    updateJobList(pGeneral->printJobList());
    pGeneral->runAllJob();
}

void MainWindow::on_pushButton_Init_MY_clicked()
{
    Job initY;
    initY.sendTo = Enums::SendTo::toLaser;
    initY.command = pLaserObj->initMotors(Enums::Y);
    initY.exp_res = "$MY:S,200";
    // we send short command to laser, and waiting resp


    pGeneral->addJob(initY);
    qDebug() << "We will send to laser this row:" << initY.command;
    _logger->info("We will send to laser this row: {}", qPrintable(initY.command));
    updateJobList(pGeneral->printJobList());
    pGeneral->runAllJob();
}

void MainWindow::on_pushButton_Cub_Circel_clicked()
{
    command = "#circle=1,2,2,0,180";
    qDebug() << "We will send to laser this row:" << command;
    _logger->info("We will send to laser this row: {}", qPrintable(command));
    pComLaserObj->SendCommand(command);
}


void MainWindow::on_pushButton_ProC_SentLaserSettings_clicked()
{
    Job laserSettings(
                Enums::SendTo::toLaser,
                "#del=%1", "DELAY_OK",1600);


    QString expTime = ui->lineEdit_ProC_ExT->text();

    command = "#del=%1";
    laserSettings.command = command.arg(expTime);

    qDebug() << "We will send to laser this row:" << command;
    _logger->info("We will send to laser this row: {}", qPrintable(command));
    pGeneral->addJob(laserSettings);
    /*
    pComLaserObj->SendCommand(command);
    Delay_MSec(pause);
    */
    command = "#step=%1";
    laserSettings.command = command.arg(ui->lineEdit_ProC_PD->text());
    laserSettings.exp_res = "STEP_OK";
    qDebug() << "We will send to laser this row:" << command;
    _logger->info("We will send to laser this row: {}", qPrintable(command));
    pGeneral->addJob(laserSettings);
    /*
    pComLaserObj->SendCommand(command);
    Delay_MSec(pause);
    */

    command = "#pow=%1";
    laserSettings.command =  command.arg(ui->lineEdit_Proc_LasPow->text());
    laserSettings.exp_res = "POWER_OK";
    qDebug() << "We will send to laser this row:" << command;
    _logger->info("We will send to laser this row: {}", qPrintable(command));
    pGeneral->addJob(laserSettings);

    updateJobList(pGeneral->printJobList());
    pGeneral->runAllJob();

    /*
    pComLaserObj->SendCommand(command);
    Delay_MSec(pause);
    */
}

void MainWindow::on_pushButton_Cub_Line_clicked()
{
    command = "#line=0,0,10,10";
    qDebug() << "We will send to laser this row:" << command;
    _logger->info("We will send to laser this row: {}", qPrintable(command));
    pComLaserObj->SendCommand(command);
}

void MainWindow::on_pushButton_Cub_AutoStart_clicked()
{
    /*
    Job line;
    line.sendTo = Enums::SendTo::toLaser;
    line.command = "#line=0,0,10,10";
    line.exp_res = "LINE_OK";
    line.timeout = 30000; // 10 sec - this can be not enought
    pGeneral->addJob(line);

    qDebug() << "We will send to laser this row:" << command;
    _logger->info("We will send to laser this row: {}", qPrintable(command));
    //pComLaserObj->SendCommand(command);

    line.command = "#line=0,10,10,0";
    pGeneral->addJob(line);
    qDebug() << "We will send to laser this row:" << command;
    _logger->info("We will send to laser this row: {}", qPrintable(command));

    Job circle;
    circle.sendTo  = Enums::SendTo::toLaser;
    circle.command = "#circle=2,5,5,0,360";
    circle.exp_res = "CIRCLE_OK";
    circle.timeout = 30000; // 10 sec - this can be not enought
    for (int i = 0; i < 20; ++i) {
           pGeneral->addJob(circle);
    }

    line.command = "#line=0,0,10,0";
    pGeneral->addJob(line);

    line.command = "#line=10,0,10,10";
    pGeneral->addJob(line);

    line.command = "#line=10,10,0,10";
    pGeneral->addJob(line);

    line.command = "#line=0,10, 0,0";
    pGeneral->addJob(line);
*/
    for (int i = 1; i <= 5; ++i) {
        auto recJobs = sqere(i*2);
        foreach (auto job , recJobs) {
            pGeneral->addJob(job);
        }
    }

    updateJobList(pGeneral->printJobList());
    pGeneral->runAllJob();
    //pComLaserObj->SendCommand(command);
}


void MainWindow::on_pushButton_Main_Start_clicked()
{
    QString LineG_code = ui->plainTextEdit_GCode->toPlainText();
    qDebug() << "We will send to laser this row:" << LineG_code;
    _logger->info("We will send to laser this row: {}", qPrintable(LineG_code));
    pComLaserObj->SendCommand(LineG_code);
}

void MainWindow::on_pushButton_Cub_Stop_clicked()
{

}

void MainWindow::on_pushButton_Main_Stop_clicked()
{
   pGeneral->clearJobList();
}
