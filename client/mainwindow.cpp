#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "client.h"
#include "loginwindow.h"
#include <QStatusBar>

#include <QDialog>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QLabel>
#include <QFontDatabase>


#include <QInputDialog>
#include <QMessageBox>
#include <QRegularExpression>

class TaskDialog : public QDialog
{
public:
    TaskDialog(const QString &title, const QString &taskText, QWidget *parent = nullptr)
        : QDialog(parent)
    {
        setWindowTitle(title);
        resize(700, 500);

        auto *layout = new QVBoxLayout(this);

        auto *taskLabel = new QLabel("Текст задачи:", this);
        layout->addWidget(taskLabel);

        auto *taskView = new QPlainTextEdit(this);
        taskView->setReadOnly(true);
        taskView->setPlainText(taskText);

        QFont mono = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        mono.setPointSize(11);
        taskView->setFont(mono);

        layout->addWidget(taskView);

        auto *answerLabel = new QLabel("Введите ответ:", this);
        layout->addWidget(answerLabel);

        answerEdit = new QLineEdit(this);
        layout->addWidget(answerEdit);

        auto *buttons = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
        connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
        layout->addWidget(buttons);



    }

    QString answer() const
    {
        return answerEdit->text();
    }

private:
    QLineEdit *answerEdit = nullptr;
};


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);


    ui->pushButton->setText("Краскал");
    ui->pushButton_2->setText("Прим");
    ui->pushButton_3->setText("Кратчайший путь");
    ui->pushButton_4->setText("Обход в глубину");

    connect(Client::getInstance(), &Client::taskReceived,
            this, &MainWindow::handleTaskReceived);
    connect(Client::getInstance(), &Client::taskResultSaved,
            this, &MainWindow::handleTaskResultSaved);
    connect(Client::getInstance(), &Client::tokenExpired,
            this, &MainWindow::handleTokenExpired);
    connect(Client::getInstance(), &Client::serverMessage,
            this, &MainWindow::handleServerMessage);
    connect(Client::getInstance(), &Client::statsReceived,
            this, &MainWindow::handleStatsReceived);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleStatsReceived(const QString &data)
{
    QStringList parts = data.split('&');
    if (parts.size() != 3) {
        QMessageBox::warning(this, "Ошибка", "Сервер прислал некорректную статистику.");
        return;
    }

    bool okTotal = false;
    bool okCorrect = false;
    bool okFailed = false;

    int total = parts[0].toInt(&okTotal);
    int correct = parts[1].toInt(&okCorrect);
    int failed = parts[2].toInt(&okFailed);

    if (!okTotal || !okCorrect || !okFailed) {
        QMessageBox::warning(this, "Ошибка", "Не удалось разобрать статистику.");
        return;
    }

    QMessageBox::information(
        this,
        "Статистика",
        QString("Всего выполнено: %1\nПравильно: %2\nНеправильно: %3")
            .arg(total)
            .arg(correct)
            .arg(failed)
        );
}


void MainWindow::requestTask(const QString &taskCommand)
{
    Client::getInstance()->requestTask(taskCommand);
}

QString MainWindow::taskTitle(const QString &taskType) const
{
    if (taskType == "kruskalsAlgorithm") return "Задание: Краскал";
    if (taskType == "primesAlgorithm") return "Задание: Прим";
    if (taskType == "isPathShortest") return "Задание: кратчайший путь";
    if (taskType == "findNodesByDepth") return "Задание: обход в глубину";
    return "Задание";
}

QString MainWindow::normalizeText(const QString &text) const
{
    QString result = text.trimmed().toLower();
    result.replace("ё", "е");
    return result.simplified();
}

QStringList MainWindow::normalizeList(const QString &text) const
{
    QStringList parts = normalizeText(text).split(QRegularExpression("[,\\s]+"), Qt::SkipEmptyParts);
    parts.sort();
    return parts;
}

bool MainWindow::answersMatch(const QString &userAnswer, const QString &correctAnswer) const
{
    QString left = normalizeText(userAnswer);
    QString right = normalizeText(correctAnswer);

    if (left == right) {
        return true;
    }

    if (right.contains(',') || left.contains(',')) {
        return normalizeList(left) == normalizeList(right);
    }

    return false;
}

void MainWindow::handleTaskReceived(const QString &taskType, int taskId, const QString &taskText, const QString &correctAnswer)
{
    QString preparedTaskText = taskText;
    preparedTaskText.replace("\\n", "\n");
    preparedTaskText.replace("\\t", "\t");

    TaskDialog dialog(taskTitle(taskType), preparedTaskText, this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QString userAnswer = dialog.answer();
    bool isCorrect = answersMatch(userAnswer, correctAnswer);

    if (isCorrect) {
        QMessageBox::information(this, "Результат", "Верно!");
    } else {
        QMessageBox::warning(this, "Результат", "Неверно.\nПравильный ответ: " + correctAnswer);
    }

    Client::getInstance()->setTaskResult(taskId, isCorrect);
}


void MainWindow::handleTaskResultSaved()
{
    statusBar()->showMessage("Результат сохранён в статистику.", 3000);
}

void MainWindow::handleTokenExpired()
{
    QMessageBox::warning(this, "Сессия истекла", "Токен истёк. Войди заново.");

    disconnect(Client::getInstance(), nullptr, this, nullptr);
    Client::getInstance()->logout();

    LoginWindow *loginWin = new LoginWindow();
    loginWin->setAttribute(Qt::WA_DeleteOnClose);
    loginWin->show();

    close();
}


void MainWindow::handleServerMessage(const QString &message)
{
    QMessageBox::warning(this, "Ответ сервера", message);
}

void MainWindow::on_pushButton_clicked()
{
    requestTask("kruskalsAlgorithm");
}

void MainWindow::on_pushButton_2_clicked()
{
    requestTask("primesAlgorithm");
}

void MainWindow::on_pushButton_3_clicked()
{
    requestTask("isPathShortest");
}

void MainWindow::on_pushButton_4_clicked()
{
    requestTask("findNodesByDepth");
}

void MainWindow::on_btn_logout_clicked()
{
    disconnect(Client::getInstance(), nullptr, this, nullptr);
    Client::getInstance()->logout();

    LoginWindow *loginWin = new LoginWindow();
    loginWin->setAttribute(Qt::WA_DeleteOnClose);
    loginWin->show();

    close();
}

void MainWindow::on_btn_stats_clicked()
{
    Client::getInstance()->getStats();
}

