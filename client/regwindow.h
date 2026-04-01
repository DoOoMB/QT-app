#ifndef REGWINDOW_H
#define REGWINDOW_H

#include <QWidget>

namespace Ui {
class RegWindow;
}

class RegWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RegWindow(QWidget *parent = nullptr);
    ~RegWindow();

private slots:
    void on_btn_register_clicked(); // Кнопка "Зарегистрироваться"
    void on_btn_back_clicked();     // Кнопка "Назад"

    void handleRegSuccess();        // Слот для сигнала regSuccess
    void handleRegError();          // Слот для сигнала regError

private:
    Ui::RegWindow *ui;
};

#endif // REGWINDOW_H
