#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pCommObj = new Communicaton();
    on_pushButton_Com_Refresh_clicked();

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
