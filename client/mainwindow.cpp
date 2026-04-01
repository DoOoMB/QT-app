#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked() {
    qDebug() << "Task 1 clicked";
    // Пока просто заглушка
}

void MainWindow::on_pushButton_2_clicked() {
    qDebug() << "Task 2 clicked";
}

void MainWindow::on_pushButton_3_clicked() {
    qDebug() << "Task 3 clicked";
}

void MainWindow::on_pushButton_4_clicked() {
    qDebug() << "Task 4 clicked";
}

void MainWindow::on_btn_logout_clicked() {
    // Возврат к окну логина
    LoginWindow *loginWin = new LoginWindow();
    loginWin->show();
    this->close();
}
