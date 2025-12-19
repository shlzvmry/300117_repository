#include "widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QStackedWidget>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDateTime>
#include <QTcpSocket>
#include <QApplication>
#include <QScreen>
#include <QJsonArray>

Widget::Widget(QWidget *parent)
    : QMainWindow(parent), stackedWidget(nullptr), loginWidget(nullptr), chatWidget(nullptr),
    ipLineEdit(nullptr), nicknameLineEdit(nullptr), loginButton(nullptr),
    chatTextEdit(nullptr), inputLineEdit(nullptr), sendButton(nullptr), exitButton(nullptr),
    userListWidget(nullptr), tcpSocket(new QTcpSocket(this)), myNickname("")
{
    setWindowTitle("聊天室客户端");
    resize(600, 600);

    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    createLoginInterface();
    createChatInterface();

    stackedWidget->addWidget(loginWidget);
    stackedWidget->addWidget(chatWidget);
    stackedWidget->setCurrentIndex(0);

    loadStyleSheet(":/style.qss");

    // 连接界面信号
    connect(loginButton, &QPushButton::clicked, this, &Widget::onLoginButtonClicked);
    connect(sendButton, &QPushButton::clicked, this, &Widget::onSendButtonClicked);
    connect(exitButton, &QPushButton::clicked, this, &Widget::onExitButtonClicked);
    // 连接网络信号
    connect(tcpSocket, &QTcpSocket::connected, this, &Widget::onConnected);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &Widget::onDisconnected);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &Widget::onReadyRead);
    // 回车发送消息
    connect(inputLineEdit, &QLineEdit::returnPressed, this, &Widget::onSendButtonClicked);
}

Widget::~Widget()
{
}

void Widget::createLoginInterface()
{
    loginWidget = new QWidget(this);
    QFormLayout *loginLayout = new QFormLayout(loginWidget);

    ipLineEdit = new QLineEdit("127.0.0.1", this);
    nicknameLineEdit = new QLineEdit(this);

    loginButton = new QPushButton("登录", this);

    loginLayout->addRow("服务器 IP:", ipLineEdit);
    loginLayout->addRow("昵称:", nicknameLineEdit);
    loginLayout->addRow("", loginButton);
}

void Widget::createChatInterface()
{
    chatWidget = new QWidget(this);
    QVBoxLayout *chatLayout = new QVBoxLayout(chatWidget);

    QLabel *infoLabel = new QLabel("谢静蕾的聊天室(2023414300117)", this);
    infoLabel->setObjectName("infoLabel");
    chatLayout->addWidget(infoLabel);

    QHBoxLayout *contentLayout = new QHBoxLayout();
    chatLayout->addLayout(contentLayout);

    chatTextEdit = new QTextEdit(this);
    chatTextEdit->setReadOnly(true);
    contentLayout->addWidget(chatTextEdit, 3);

    userListWidget = new QListWidget(this);
    contentLayout->addWidget(userListWidget, 1);

    QHBoxLayout *inputLayout = new QHBoxLayout();
    chatLayout->addLayout(inputLayout);

    inputLineEdit = new QLineEdit(this);
    sendButton = new QPushButton("发送", this);
    exitButton = new QPushButton("退出", this);

    inputLayout->addWidget(inputLineEdit);
    inputLayout->addWidget(sendButton);
    inputLayout->addWidget(exitButton);
}

void Widget::onLoginButtonClicked()
{
    QString ip = ipLineEdit->text().trimmed();
    QString nickname = nicknameLineEdit->text().trimmed();

    if (ip.isEmpty() || nickname.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请填写服务器IP和昵称！");
        return;
    }

    myNickname = nickname;

    tcpSocket->connectToHost(ip, 8888);
    if (!tcpSocket->waitForConnected(3000)) {
        appendChatMessage("[系统] 连接服务器超时或失败。");
    } else {
        appendChatMessage("[系统] 正在尝试登录...");
    }
}

void Widget::onConnected()
{
    appendChatMessage("[系统] 已连接到服务器。");
    QJsonObject loginMsg;
    loginMsg["type"] = "login";
    loginMsg["nickname"] = myNickname;
    QJsonDocument doc(loginMsg);
    QByteArray dataToSend = doc.toJson(QJsonDocument::Compact) + "\n";
    tcpSocket->write(dataToSend);
    tcpSocket->flush();
}

void Widget::onDisconnected()
{
    appendChatMessage("[系统] 与服务器断开连接。");
    stackedWidget->setCurrentIndex(0);
    userListWidget->clear();
    myNickname.clear();
}

void Widget::onErrorOccurred(QAbstractSocket::SocketError socketError)
{
    QString errorStr;
    switch (socketError) {
    case QAbstractSocket::ConnectionRefusedError:
        errorStr = "连接被服务器拒绝。"; break;
    case QAbstractSocket::RemoteHostClosedError:
        errorStr = "服务器关闭了连接。"; break;
    case QAbstractSocket::HostNotFoundError:
        errorStr = "找不到服务器主机。"; break;
    case QAbstractSocket::SocketAccessError:
        errorStr = "没有权限访问套接字。"; break;
    case QAbstractSocket::SocketResourceError:
        errorStr = "套接字资源不足。"; break;
    case QAbstractSocket::SocketTimeoutError:
        errorStr = "套接字操作超时。"; break;
    case QAbstractSocket::DatagramTooLargeError:
        errorStr = "数据报太大。"; break;
    case QAbstractSocket::NetworkError:
        errorStr = "网络错误。"; break;
    case QAbstractSocket::AddressInUseError:
        errorStr = "地址已被使用。"; break;
    case QAbstractSocket::SocketAddressNotAvailableError:
        errorStr = "套接字地址不可用。"; break;
    case QAbstractSocket::UnsupportedSocketOperationError:
        errorStr = "不支持的套接字操作。"; break;
    case QAbstractSocket::UnfinishedSocketOperationError:
        errorStr = "未完成的套接字操作。"; break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        errorStr = "代理需要身份验证。"; break;
    case QAbstractSocket::SslHandshakeFailedError:
        errorStr = "SSL握手失败。"; break;
    case QAbstractSocket::ProxyConnectionRefusedError:
        errorStr = "代理连接被拒绝。"; break;
    case QAbstractSocket::ProxyConnectionClosedError:
        errorStr = "代理连接关闭。"; break;
    case QAbstractSocket::ProxyConnectionTimeoutError:
        errorStr = "代理连接超时。"; break;
    case QAbstractSocket::ProxyNotFoundError:
        errorStr = "找不到代理。"; break;
    case QAbstractSocket::ProxyProtocolError:
        errorStr = "代理协议错误。"; break;
    default:
        errorStr = QString("未知错误 (%1)。").arg(socketError);
    }
    appendChatMessage(QString("[错误] %1").arg(errorStr));
    if (stackedWidget->currentIndex() == 1) {
        stackedWidget->setCurrentIndex(0);
    }
}

void Widget::onReadyRead()
{
    while (tcpSocket->canReadLine()) {
        QByteArray jsonData = tcpSocket->readLine().trimmed();
        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            appendChatMessage(QString("[错误] 解析服务器消息失败: %1").arg(parseError.errorString()));
            appendChatMessage(QString("原始数据: %1").arg(QString(jsonData)));
            continue;
        }

        if (!jsonDoc.isObject()) {
            appendChatMessage("[警告] 收到非JSON对象数据。");
            continue;
        }

        QJsonObject obj = jsonDoc.object();
        handleServerMessage(obj);
    }
}

void Widget::handleServerMessage(const QJsonObject &obj)
{
    QString type = obj["type"].toString();

    if (type == "login_success") {
        stackedWidget->setCurrentIndex(1);
        appendChatMessage("[系统] 登录成功，欢迎来到聊天室！");
    } else if (type == "login_failed") {
        QString reason = obj["reason"].toString();
        appendChatMessage(QString("[系统] 登录失败: %1").arg(reason));
        tcpSocket->disconnectFromHost();
    } else if (type == "user_joined") {
        QString nickname = obj["nickname"].toString();
        appendChatMessage(QString("%1 加入了聊天室").arg(nickname));
        if (userListWidget && !nickname.isEmpty()) {
            QList<QListWidgetItem*> existingItems = userListWidget->findItems(nickname, Qt::MatchExactly);
            if (existingItems.isEmpty()) {
                userListWidget->addItem(nickname);
            } else {
                appendChatMessage(QString("【警告】尝试添加已存在的用户到列表: %1").arg(nickname));
            }
        }
    } else if (type == "user_left") {
        QString nickname = obj["nickname"].toString();
        appendChatMessage(QString("%1 离开了聊天室").arg(nickname));
        if (userListWidget) {
            QList<QListWidgetItem*> items = userListWidget->findItems(nickname, Qt::MatchExactly);
            for (QListWidgetItem* item : items) {
                delete userListWidget->takeItem(userListWidget->row(item));
            }
        }
    } else if (type == "chat_message") {
        QString sender = obj["sender"].toString();
        QString message = obj["message"].toString();
        QString displayMessage = QString("%1: %2").arg(sender).arg(message);
        appendChatMessage(displayMessage);
    } else if (type == "user_list") {
        QJsonArray usersArray = obj["users"].toArray();
        QStringList users;
        for (const QJsonValue &value : usersArray) {
            users << value.toString();
        }
        updateUserList(users);
    } else {
        appendChatMessage(QString("[警告] 收到未知类型服务器消息: %1").arg(type));
    }
}

void Widget::onSendButtonClicked()
{
    QString message = inputLineEdit->text().trimmed();
    if (message.isEmpty() || !tcpSocket || tcpSocket->state() != QAbstractSocket::ConnectedState) {
        return;
    }

    QJsonObject chatMsg;
    chatMsg["type"] = "chat_message";
    chatMsg["message"] = message;
    QJsonDocument doc(chatMsg);
    QByteArray dataToSend = doc.toJson(QJsonDocument::Compact) + "\n";
    tcpSocket->write(dataToSend);
    tcpSocket->flush();

    inputLineEdit->clear();
}

void Widget::onExitButtonClicked()
{
    if (tcpSocket->state() == QAbstractSocket::ConnectedState) {
        tcpSocket->disconnectFromHost();
    }
    stackedWidget->setCurrentIndex(0);
    chatTextEdit->clear();
    userListWidget->clear();
    myNickname.clear();
}

void Widget::loadStyleSheet(const QString &sheetName)
{
    QFile file(sheetName);
    if (file.open(QIODevice::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
    } else {
        qDebug() << "无法打开样式表文件:" << sheetName << file.errorString();
    }
}

void Widget::updateUserList(const QStringList &users)
{
    userListWidget->clear();
    for (const QString &user : users) {
        QListWidgetItem *item = new QListWidgetItem(user, userListWidget);
        if (user == myNickname) {
            QFont font = item->font();
            font.setBold(true);
            item->setFont(font);
            item->setText(QString("%1 (我)").arg(user));
        }
    }
}

void Widget::appendChatMessage(const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    chatTextEdit->append(QString("[%1] %2").arg(timestamp).arg(message));
}
