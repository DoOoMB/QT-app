#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QStringList>

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
    void on_btn_stats_clicked();
    void handleStatsReceived(const QString &data);

    void handleTaskReceived(const QString &taskType, int taskId, const QString &taskText, const QString &correctAnswer);
    void handleTaskResultSaved();
    void handleTokenExpired();
    void handleServerMessage(const QString &message);

private:
    Ui::MainWindow *ui;
    void requestTask(const QString &taskCommand);
    QString taskTitle(const QString &taskType) const;
    QString normalizeText(const QString &text) const;
    QStringList normalizeList(const QString &text) const;
    bool answersMatch(const QString &userAnswer, const QString &correctAnswer) const;
};
#endif // MAINWINDOW_H
