#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "client.h"
#include "regwindow.h"
#include "mainwindow.h"
#include <QMessageBox>

LoginWindow::LoginWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
    connect(Client::getInstance(), &Client::authSuccess, this, &LoginWindow::handleAuthSuccess);
    connect(Client::getInstance(), &Client::authError, this, &LoginWindow::handleAuthError);
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::on_btn_login_clicked() {
    QString login = ui->lineEdit_login->text();
    QString pass = ui->lineEdit_password->text();

    if (login.isEmpty() || pass.isEmpty()) {
        QMessageBox::warning(this, "Внимание", "Заполните все поля!");
        return;
    }

    // Отправляем запрос через Синглтон
    Client::getInstance()->auth(login, pass);
}

void LoginWindow::on_btn_reg_clicked() {
    RegWindow *regWin = new RegWindow();

    // Когда окно регистрации закроется, мы хотим снова увидеть это окно
    connect(regWin, &RegWindow::destroyed, this, &LoginWindow::show);

    regWin->setAttribute(Qt::WA_DeleteOnClose); // Авто-удаление памяти при закрытии
    regWin->show();
    this->hide();
}

void LoginWindow::handleAuthSuccess()
{
    qDebug() << "Авторизация прошла успешно!";

    disconnect(Client::getInstance(), nullptr, this, nullptr);

    MainWindow *mainWin = new MainWindow();
    mainWin->setAttribute(Qt::WA_DeleteOnClose);
    mainWin->show();

    this->close();
}


void LoginWindow::handleAuthError() {
    QMessageBox::critical(this, "Ошибка", "Неверный логин или пароль!");
}
