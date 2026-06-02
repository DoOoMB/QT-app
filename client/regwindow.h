#ifndef REGWINDOW_H
#define REGWINDOW_H

#include <QWidget>

namespace Ui {
class RegWindow;
}

/**
 * @class RegWindow
 * @brief Simple registration dialog used to create a new account.
 */
class RegWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RegWindow(QWidget *parent = nullptr);
    ~RegWindow();

private slots:
    /** Handle the "Register" button click. */
    void on_btn_register_clicked(); // Кнопка "Зарегистрироваться"
    /** Go back to previous screen. */
    void on_btn_back_clicked();     // Кнопка "Назад"

    /** Called when registration succeeds. */
    void handleRegSuccess();        // Слот для сигнала regSuccess
    /** Called when registration fails. */
    void handleRegError();          // Слот для сигнала regError

private:
    Ui::RegWindow *ui;
};

#endif // REGWINDOW_H
