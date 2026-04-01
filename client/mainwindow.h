#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Слоты-заглушки для кнопок
    void on_pushButton_clicked();   // Task 1
    void on_pushButton_2_clicked(); // Task 2
    void on_pushButton_3_clicked(); // Task 3
    void on_pushButton_4_clicked(); // Task 4
    void on_btn_logout_clicked();   // Кнопка выхода

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
