#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>

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

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    Ui::MainWindow *ui;
    double m_firstOperand;
    double m_secondOperand;
    QString m_pendingOperator;
    bool m_waitingForOperand;
    QString m_displayBuffer;  // 显示缓冲区，用于显示完整表达式
    QString m_currentInput;   // 当前输入的数字

    void calculate();
    void updateDisplay();
    void appendToDisplay(const QString &text);
    void resetCalculator();

private slots:
    void onDigitClicked();
    void onOperatorClicked();
    void onClearClicked();
    void onBackspaceClicked();
    void onEqualClicked();
    void onDecimalClicked();
    void onPercentClicked();
};
#endif // MAINWINDOW_H
