#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "communicaton.h"

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

private:
    Ui::MainWindow *ui;

    Communicaton *pCommObj;
};

#endif // MAINWINDOW_H
