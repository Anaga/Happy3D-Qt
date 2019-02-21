#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pCommObj = new Communicaton();
    on_pushButton_Com_Refresh_clicked();
    ui->pushButton_Com_Disconnect->setVisible(false);

    connect( pCommObj, SIGNAL(readDataFromCom(QByteArray)), this, SLOT(getDataFromCom(const QByteArray)));

    pLaserObj = new LaserControl;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_Com_Refresh_clicked()
{
    ui->comboBox_Com_Select->clear();
    QStringList portList;
    portList = pCommObj->GetInfo();
    if (portList.size()>0) {
        for (auto port :  portList) {
            ui->comboBox_Com_Select->addItem(port);
        }
    }
}

void MainWindow::on_pushButton_Com_Connect_clicked()
{
    QString qsPortName = ui->comboBox_Com_Select->currentText();
    QString qsTemp = "%1 %2";
    if (pCommObj->OpenConnection(qsPortName)){
        qsTemp = qsTemp.arg(qsPortName).arg("is open");
        ui->pushButton_Com_Disconnect->setVisible(true);
        ui->pushButton_Com_Connect->setVisible(false);
        //ui->pushButton_Com_Connect->setText("Disconnect");
    } else {
        qsTemp = qsTemp.arg(qsPortName).arg("not open");
        // ui->pushButton_Com_Connect->setText("Connect");
    }
    ui->label_Com_Status->setText(qsTemp);
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

void MainWindow::on_pushButton_Com_Disconnect_clicked()
{
    QString qsPortName = ui->comboBox_Com_Select->currentText();
    QString qsTemp = "%1 %2";
    if (pCommObj->CloseConnection(qsPortName)){
        qsTemp = qsTemp.arg(qsPortName).arg("is closed");
        ui->pushButton_Com_Disconnect->setVisible(false);
        ui->pushButton_Com_Connect->setVisible(true);
        //ui->pushButton_Com_Connect->setText("Disconnect");
    } else {
        qsTemp = qsTemp.arg(qsPortName).arg("not closed");
        // ui->pushButton_Com_Connect->setText("Connect");
    }
    ui->label_Com_Status->setText(qsTemp);
}

void MainWindow::getDataFromCom(const QByteArray &arg1)
{
    // qDebug() << __PRETTY_FUNCTION__;
    QString qsTemp(arg1);
    qsTemp = qsTemp.trimmed();
    ui->textBrowser_Main_Trans->append(qsTemp);
}

void MainWindow::on_pushButton_ProC_StopLaser_clicked()
{
    QString qsTemp = pLaserObj->stopLaser();
    QString qsPortName = ui->comboBox_Com_Select->currentText();
    qDebug() << "We will send to laser this row:" << qsTemp;
    pCommObj->SendCommand(qsPortName, qsTemp);
}

void MainWindow::on_pushButton_ProcC_StopMotor_clicked()
{
    QString qsTemp = pLaserObj->stopMotor();
    QString qsPortName = ui->comboBox_Com_Select->currentText();
    qDebug() << "We will send to laser this row:" << qsTemp;
    pCommObj->SendCommand(qsPortName, qsTemp);
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
    long speed =0;
    long dist =0;
    speed = ui->lineEdit_MotC_HorSpeed->text().toLong();
    dist =  ui->lineEdit_MotC_HorDist->text().toLong();
    QString command = pLaserObj->moveWiper(Left,dist,speed);
    QString qsPortName = ui->comboBox_Com_Select->currentText();
    qDebug() << "We will send to laser this row:" << command;
    pCommObj->SendCommand(qsPortName, command);
}

void MainWindow::on_pushButton_MotC_Rigth_clicked()
{
    long speed =0;
    long dist =0;
    speed = ui->lineEdit_MotC_HorSpeed->text().toLong();
    dist =  ui->lineEdit_MotC_HorDist->text().toLong();
    QString command = pLaserObj->moveWiper(Right,dist,speed);
    QString qsPortName = ui->comboBox_Com_Select->currentText();
    qDebug() << "We will send to laser this row:" << command;
    pCommObj->SendCommand(qsPortName, command);
}

void MainWindow::on_pushButton_MotC_Up_clicked()
{
    long speed =0;
    long dist =0;
    speed = ui->lineEdit_MotC_VertDist->text().toLong();
    dist =  ui->lineEdit_MotC_VertDist->text().toLong();
    QString command = pLaserObj->movePlate(Up,dist,speed);
    QString qsPortName = ui->comboBox_Com_Select->currentText();
    qDebug() << "We will send to laser this row:" << command;
    pCommObj->SendCommand(qsPortName, command);
}

void MainWindow::on_pushButton_MotC__Down_clicked()
{
    long speed =0;
    long dist =0;
    speed = ui->lineEdit_MotC_VertDist->text().toLong();
    dist =  ui->lineEdit_MotC_VertDist->text().toLong();
    QString command = pLaserObj->movePlate(Down,dist,speed);
    QString qsPortName = ui->comboBox_Com_Select->currentText();
    qDebug() << "We will send to laser this row:" << command;
    pCommObj->SendCommand(qsPortName, command);
}
