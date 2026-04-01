#include "regwindow.h"
#include "ui_regwindow.h"

RegWindow::RegWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RegWindow)
{
    ui->setupUi(this);
    connect(Client::getInstance(), &Client::regSuccess, this, &RegWindow::handleRegSuccess);
    connect(Client::getInstance(), &Client::regError, this, &RegWindow::handleRegError);
}

RegWindow::~RegWindow()
{
    delete ui;
}

void RegWindow::on_btn_register_clicked() {
    QString login = ui->lineEdit_login->text();
    QString pass = ui->lineEdit_password->text();

    if (login.isEmpty() || pass.isEmpty()) {
        QMessageBox::warning(this, "Внимание", "Поля не могут быть пустыми!");
        return;
    }

    // Вызываем метод регистрации в синглтоне
    Client::getInstance()->reg(login, pass);
}

void RegWindow::handleRegSuccess() {
    QMessageBox::information(this, "Успех", "Регистрация прошла успешно! Теперь войдите.");
    on_btn_back_clicked(); // Возвращаемся назад
}

void RegWindow::handleRegError() {
    QMessageBox::critical(this, "Ошибка", "Логин уже занят или произошла ошибка сервера.");
}

void RegWindow::on_btn_back_clicked() {
    // Чтобы вернуться, нам нужно просто закрыть это окно.
    // Но важно, чтобы в LoginWindow мы его не удаляли из памяти при переходе.
    this->close();
}
