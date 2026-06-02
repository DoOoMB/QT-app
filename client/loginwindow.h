#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>

#namespace Ui {
namespace Ui {
class LoginWindow;
}

/**
 * @class LoginWindow
 * @brief Simple login dialog used to enter credentials.
 *
 * Emits or reacts to signals from `Client` to show auth result.
 */
class LoginWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private slots:
    /** Slot for the "Login" button. */
    void on_btn_login_clicked();      // Слот кнопки "Войти"
    /** Slot for the "Register" button. */
    void on_btn_reg_clicked();        // Слот кнопки "Регистрация"

    /** Called when authentication succeeds (token received). */
    void handleAuthSuccess();         // Если сервер прислал токен
    /** Called when authentication fails. */
    void handleAuthError();           // Если логин/пароль неверны


private:
    Ui::LoginWindow *ui;
};

#endif // LOGINWINDOW_H



