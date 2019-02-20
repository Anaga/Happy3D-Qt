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

bool MainWindow::inputCheck(QString text)
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
    qreal fVal;
    QString qsTemp = "Can't convert %1 to Double!";

    fVal = text.toDouble(&bOk);
    if (!bOk){
        qsTemp = qsTemp.arg(text);
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(qsTemp);
        msgBox.exec();
    }
    qDebug() << "fVal is " << fVal;
    return bOk;
}


void MainWindow::on_lineEdit_ProC_PD_textEdited(const QString &arg1)
{
    if (!inputCheck(arg1)) {
        ui->lineEdit_ProC_PD->clear();
    }
}

void MainWindow::on_lineEdit_ProC_ExT_textEdited(const QString &arg1)
{
    if (!inputCheck(arg1)) {
        ui->lineEdit_ProC_ExT->clear();
    }
}

void MainWindow::on_lineEdit_Proc_LasPow_textEdited(const QString &arg1)
{
    if (!inputCheck(arg1)) {
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
