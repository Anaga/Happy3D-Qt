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

void MainWindow::initMotors()
{
    _logger->info("initMotors");
    //Delay_MSec(400);
    ui->pushButton_Init_MX->click();

    Task initX("#mx=S,200", "$MX:S,200");
    initX.setSendTo(Enums::SendTo::toLaser);
    //initX.setCommand("#mx=S,200");
    //initX.setExp_res("$MX:S,200");
    _logger->info(qPrintable(initX.print()));
    Delay_MSec(100);
    Task initY("#my=S,200", "$MY:S,200");
    _logger->info(qPrintable(initY.print()));
    ui->pushButton_Init_MY->click();
    //_logger->info(qPrintable(initY.print()));
}

void MainWindow::recoaterSeq()
{
    _logger->info(__PRETTY_FUNCTION__);

    // we will use Delay_MSec to simylate delays.
        unsigned int timeout = 500; //milliseconds

        pComPresObj->SendCommand("R"); //Right up
        Delay_MSec(timeout);
        pComPresObj->SendCommand("F"); //left down
        Delay_MSec(timeout);

        //Motor2 run to the right end
        long distance = 13000;
        long speed = 1600;
        command = pLaserObj->moveMotors(Enums::Right, distance, speed);
        qDebug() << "We will send to laser this row:" << command;
        pComLaserObj->SendCommand(command);
        Delay_MSec(9000);

        pComPresObj->SendCommand("I"); //Right down
        Delay_MSec(timeout);

        pComPresObj->SendCommand("L"); //left down
        Delay_MSec(timeout);

        //Motor2 run to the push position
        // MoveMotor2("8300", "y", "2");
        distance = 8300;
        command = pLaserObj->moveMotors(Enums::Left, distance, speed);
        qDebug() << "We will send to laser this row:" << command;
        pComLaserObj->SendCommand(command);
        Delay_MSec(6000);

        pComPresObj->SendCommand("D"); //Push
        Delay_MSec(timeout);

        //Motor1 down one layer
        // MoveMotor1(Motor1MoveText.Text, "x", "1");
        qsTemp = ui->lineEdit_MotC_VertDist->text();
        distance = qsTemp.toLong();
        command = pLaserObj->moveMotors(Enums::Down, distance, speed);
        qDebug() << "We will send to laser this row:" << command;
        pComLaserObj->SendCommand(command);
        Delay_MSec(timeout);

        pComPresObj->SendCommand("U"); //Pull
        Delay_MSec(timeout);

        pComPresObj->SendCommand("H"); //Hold
        Delay_MSec(timeout);

        //MoveMotor2("4700", "y", "2");
        distance = 4700;
        command = pLaserObj->moveMotors(Enums::Left, distance, speed);
        qDebug() << "We will send to laser this row:" << command;
        pComLaserObj->SendCommand(command);
        Delay_MSec(4000);

        pComPresObj->SendCommand("F"); //left down


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
    command = pLaserObj->initMotors(Enums::X);
    qDebug() << "We will send to laser this row:" << command;
    _logger->info("We will send to laser this row: {}", qPrintable(command));
    pComLaserObj->SendCommand(command);
}

void MainWindow::on_pushButton_Init_MY_clicked()
{
    command = pLaserObj->initMotors(Enums::Y);
    qDebug() << "We will send to laser this row:" << command;
    _logger->info("We will send to laser this row: {}", qPrintable(command));
    pComLaserObj->SendCommand(command);
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
    unsigned int pause = 1600;

    QString expTime = ui->lineEdit_ProC_ExT->text();

    command = "#del=%1";
    command = command.arg(expTime);

    qDebug() << "We will send to laser this row:" << command;
    _logger->info("We will send to laser this row: {}", qPrintable(command));
    pComLaserObj->SendCommand(command);
    Delay_MSec(pause);

    command = "#step=%1";
    command = command.arg(ui->lineEdit_ProC_PD->text());
    qDebug() << "We will send to laser this row:" << command;
    _logger->info("We will send to laser this row: {}", qPrintable(command));
    pComLaserObj->SendCommand(command);
    Delay_MSec(pause);


    command = "#pow=%1";
    command = command.arg(ui->lineEdit_Proc_LasPow->text());
    qDebug() << "We will send to laser this row:" << command;
    _logger->info("We will send to laser this row: {}", qPrintable(command));
    pComLaserObj->SendCommand(command);
    Delay_MSec(pause);
}

void MainWindow::on_pushButton_Cub_Line_clicked()
{
    command = "#line=1,1,5,5";
    qDebug() << "We will send to laser this row:" << command;
    _logger->info("We will send to laser this row: {}", qPrintable(command));
    pComLaserObj->SendCommand(command);
}

void MainWindow::on_pushButton_Cub_AutoStart_clicked()
{
    command = "#line=0,0,10,10";
    qDebug() << "We will send to laser this row:" << command;
    _logger->info("We will send to laser this row: {}", qPrintable(command));
    pComLaserObj->SendCommand(command);

    command = "#line=0,10,10,0";
    qDebug() << "We will send to laser this row:" << command;
    _logger->info("We will send to laser this row: {}", qPrintable(command));
    pComLaserObj->SendCommand(command);
}

