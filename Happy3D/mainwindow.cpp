#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pCommObj = new Communicaton();
    on_pushButton_Com_Refresh_clicked();

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
    } else {
        qsTemp = qsTemp.arg(qsPortName).arg("not open");
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

    fVal = text.toDouble(&bOk);
    if (!bOk){
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("Can't convert this to Double!");
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
