#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private slots:
    void on_btn_login_clicked();      // Слот кнопки "Войти"
    void on_btn_reg_clicked();        // Слот кнопки "Регистрация"

    void handleAuthSuccess();         // Если сервер прислал токен
    void handleAuthError();           // Если логин/пароль неверны


private:
    Ui::LoginWindow *ui;
};

#endif // LOGINWINDOW_H



