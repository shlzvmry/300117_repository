#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_firstOperand(0)
    , m_secondOperand(0)
    , m_waitingForOperand(true)
    , m_displayBuffer("")
    , m_currentInput("0")
{
    ui->setupUi(this);
    // 设置样式表
    setStyleSheet(
        // 主窗口背景
        "QMainWindow { background-color: #2b2b2b; }"

        // 显示屏样式
        "QLineEdit#display {"
        "   background-color: #000000;"
        "   color: #ffffff;"
        "   border: 2px solid #555555;"
        "   border-radius: 5px;"
        "   padding: 5px;"
        "   font-size: 20px;"
        "}"

        // 数字按钮样式
        "QPushButton {"
        "   background-color: #666666;"
        "   color: white;"
        "   border: 1px solid #555555;"
        "   border-radius: 5px;"
        "   font-size: 16px;"
        "   min-width: 40px;"
        "   min-height: 40px;"
        "}"

        // 运算符按钮样式
        "QPushButton[objectName^=\"btn_add\"],"
        "QPushButton[objectName^=\"btn_subtract\"],"
        "QPushButton[objectName^=\"btn_multiply\"],"
        "QPushButton[objectName^=\"btn_divide\"] {"
        "   background-color: #ff9500;"
        "   color: white;"
        "}"

        // 功能按钮样式
        "QPushButton[objectName^=\"btn_clear\"],"
        "QPushButton[objectName^=\"btn_backspace\"] {"
        "   background-color: #a6a6a6;"
        "   color: black;"
        "}"

        // 按钮悬停效果
        "QPushButton:hover {"
        "   background-color: #777777;"
        "}"

        // 运算符按钮悬停效果
        "QPushButton[objectName^=\"btn_add\"]:hover,"
        "QPushButton[objectName^=\"btn_subtract\"]:hover,"
        "QPushButton[objectName^=\"btn_multiply\"]:hover,"
        "QPushButton[objectName^=\"btn_divide\"]:hover {"
        "   background-color: #ffb143;"
        "}"
        );

    // 设置显示屏
    QFont font = ui->display->font();
    font.setPointSize(20);
    ui->display->setFont(font);
    ui->display->setAlignment(Qt::AlignRight);
    ui->display->setReadOnly(true);

    updateDisplay();

    // 连接数字按钮
    connect(ui->btn_0, &QPushButton::clicked, this, &MainWindow::onDigitClicked);
    connect(ui->btn_1, &QPushButton::clicked, this, &MainWindow::onDigitClicked);
    connect(ui->btn_2, &QPushButton::clicked, this, &MainWindow::onDigitClicked);
    connect(ui->btn_3, &QPushButton::clicked, this, &MainWindow::onDigitClicked);
    connect(ui->btn_4, &QPushButton::clicked, this, &MainWindow::onDigitClicked);
    connect(ui->btn_5, &QPushButton::clicked, this, &MainWindow::onDigitClicked);
    connect(ui->btn_6, &QPushButton::clicked, this, &MainWindow::onDigitClicked);
    connect(ui->btn_7, &QPushButton::clicked, this, &MainWindow::onDigitClicked);
    connect(ui->btn_8, &QPushButton::clicked, this, &MainWindow::onDigitClicked);
    connect(ui->btn_9, &QPushButton::clicked, this, &MainWindow::onDigitClicked);

    // 连接运算符按钮
    connect(ui->btn_add, &QPushButton::clicked, this, &MainWindow::onOperatorClicked);
    connect(ui->btn_subtract, &QPushButton::clicked, this, &MainWindow::onOperatorClicked);
    connect(ui->btn_multiply, &QPushButton::clicked, this, &MainWindow::onOperatorClicked);
    connect(ui->btn_divide, &QPushButton::clicked, this, &MainWindow::onOperatorClicked);

    // 连接功能按钮
    connect(ui->btn_clear, &QPushButton::clicked, this, &MainWindow::onClearClicked);
    connect(ui->btn_backspace, &QPushButton::clicked, this, &MainWindow::onBackspaceClicked);
    connect(ui->btn_equal, &QPushButton::clicked, this, &MainWindow::onEqualClicked);
    connect(ui->btn_decimal, &QPushButton::clicked, this, &MainWindow::onDecimalClicked);
    connect(ui->btn_percent, &QPushButton::clicked, this, &MainWindow::onPercentClicked);

    setFocusPolicy(Qt::StrongFocus);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateDisplay()
{
    if (m_displayBuffer.isEmpty()) {
        ui->display->setText(m_currentInput);
    } else {
        ui->display->setText(m_displayBuffer + m_currentInput);
    }
}

void MainWindow::appendToDisplay(const QString &text)
{
    m_displayBuffer += text;
    updateDisplay();
}

void MainWindow::resetCalculator()
{
    m_firstOperand = 0;
    m_secondOperand = 0;
    m_pendingOperator.clear();
    m_waitingForOperand = true;
    m_displayBuffer.clear();
    m_currentInput = "0";
}

// 数字按钮点击处理
void MainWindow::onDigitClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    QString digitValue = clickedButton->text();

    if (m_waitingForOperand) {
        m_currentInput = digitValue;
        m_waitingForOperand = false;
    } else {
        if (m_currentInput == "0") {
            m_currentInput = digitValue;
        } else {
            m_currentInput += digitValue;
        }
    }

    updateDisplay();
}

// 运算符按钮点击处理
void MainWindow::onOperatorClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    QString newOperator = clickedButton->text();

    // 如果不是等待新操作数状态，说明正在输入数字，需要先处理这个数字
    if (!m_waitingForOperand) {
        // 将当前输入的数字保存到显示缓冲区
        if (m_displayBuffer.isEmpty()) {
            m_displayBuffer = m_currentInput + " " + newOperator + " ";
        } else {
            m_displayBuffer += m_currentInput + " " + newOperator + " ";
        }

        // 如果有待处理的运算符，先计算
        if (!m_pendingOperator.isEmpty()) {
            m_secondOperand = m_currentInput.toDouble();
            calculate();
            m_currentInput = QString::number(m_firstOperand);
        } else {
            m_firstOperand = m_currentInput.toDouble();
        }
    } else {
        // 如果是在等待新操作数，只更新运算符
        if (!m_displayBuffer.isEmpty()) {
            // 找到最后一个运算符并替换
            QStringList parts = m_displayBuffer.split(" ");
            if (parts.size() >= 2) {
                parts[parts.size() - 2] = newOperator;  // 替换倒数第二个元素（运算符）
                m_displayBuffer = parts.join(" ") + " ";
            }
        }
    }

    m_pendingOperator = newOperator;
    m_waitingForOperand = true;
    updateDisplay();
}

// 等号按钮点击处理
void MainWindow::onEqualClicked()
{
    if (m_pendingOperator.isEmpty()) return;

    // 使用新的优先级计算函数
    calculate();

    // 显示完整表达式和结果
    QString resultDisplay = m_displayBuffer + m_currentInput + " = " + QString::number(m_firstOperand);
    ui->display->setText(resultDisplay);

    m_currentInput = QString::number(m_firstOperand);
    m_displayBuffer.clear();
    m_pendingOperator.clear();
    m_waitingForOperand = true;
}

// 清除
void MainWindow::onClearClicked()
{
    resetCalculator();
    updateDisplay();
}

// 退格
void MainWindow::onBackspaceClicked()
{
    if (m_waitingForOperand) {
        return;
    }

    m_currentInput.chop(1);
    if (m_currentInput.isEmpty()) {
        m_currentInput = "0";
        m_waitingForOperand = true;
    }

    updateDisplay();
}

// 小数点
void MainWindow::onDecimalClicked()
{
    if (m_waitingForOperand) {
        m_currentInput = "0";
        m_waitingForOperand = false;
    }

    if (!m_currentInput.contains('.')) {
        m_currentInput += ".";
    }

    updateDisplay();
}

// 百分比
void MainWindow::onPercentClicked()
{
    double currentValue = m_currentInput.toDouble();
    double percentValue = currentValue / 100.0;

    m_currentInput = QString::number(percentValue);
    updateDisplay();
}

// 计算函数
void MainWindow::calculate()
{
    // 将显示缓冲区解析为表达式
    QString expression = m_displayBuffer + m_currentInput;
    QStringList tokens = expression.split(" ", Qt::SkipEmptyParts);

    if (tokens.size() < 3) return;

    // 先处理所有乘除运算
    for (int i = 1; i < tokens.size() - 1; i += 2) {
        if (tokens[i] == "×" || tokens[i] == "÷") {
            double left = tokens[i-1].toDouble();
            double right = tokens[i+1].toDouble();
            double result = 0;

            if (tokens[i] == "×") {
                result = left * right;
            } else {
                if (right == 0) {
                    ui->display->setText("错误: 除零");
                    resetCalculator();
                    return;
                }
                result = left / right;
            }

            // 替换这三个token为结果
            tokens[i-1] = QString::number(result);
            tokens.removeAt(i);
            tokens.removeAt(i);
            i -= 2; // 调整索引
        }
    }

    // 再处理加减运算
    double finalResult = tokens[0].toDouble();
    for (int i = 1; i < tokens.size() - 1; i += 2) {
        double operand = tokens[i+1].toDouble();
        if (tokens[i] == "+") {
            finalResult += operand;
        } else if (tokens[i] == "-") {
            finalResult -= operand;
        }
    }

    m_firstOperand = finalResult;
}

// 键盘事件
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_0: ui->btn_0->click(); break;
    case Qt::Key_1: ui->btn_1->click(); break;
    case Qt::Key_2: ui->btn_2->click(); break;
    case Qt::Key_3: ui->btn_3->click(); break;
    case Qt::Key_4: ui->btn_4->click(); break;
    case Qt::Key_5: ui->btn_5->click(); break;
    case Qt::Key_6: ui->btn_6->click(); break;
    case Qt::Key_7: ui->btn_7->click(); break;
    case Qt::Key_8: ui->btn_8->click(); break;
    case Qt::Key_9: ui->btn_9->click(); break;
    case Qt::Key_Plus: ui->btn_add->click(); break;
    case Qt::Key_Minus: ui->btn_subtract->click(); break;
    case Qt::Key_Asterisk: ui->btn_multiply->click(); break;
    case Qt::Key_Slash: ui->btn_divide->click(); break;
    case Qt::Key_Equal:
    case Qt::Key_Enter:
    case Qt::Key_Return: ui->btn_equal->click(); break;
    case Qt::Key_Backspace: ui->btn_backspace->click(); break;
    case Qt::Key_Escape: ui->btn_clear->click(); break;
    case Qt::Key_Period: ui->btn_decimal->click(); break;
    case Qt::Key_Percent: ui->btn_percent->click(); break;
    default: QMainWindow::keyPressEvent(event);
    }
}
