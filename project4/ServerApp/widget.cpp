#include "widget.h"
#include <QFile>
#include <QDateTime>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QApplication>
#include <QScreen>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTcpSocket>
#include <Qt>
#include <QMetaMethod>

Widget::Widget(QWidget *parent)
    : QMainWindow(parent)
    , tcpServer(new QTcpServer(this))
    , infoLabel(nullptr)
    , logTextEdit(nullptr)
    , stopButton(nullptr)
{
    // 设置窗口标题
    setWindowTitle("聊天室服务器");
    resize(600, 600);

    // 创建中央部件和布局
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    // 创建信息标签
    infoLabel = new QLabel("谢静蕾的聊天室(2023414300117)", this);
    infoLabel->setObjectName("infoLabel");
    layout->addWidget(infoLabel);

    logTextEdit = new QTextEdit(this);
    logTextEdit->setReadOnly(true);
    layout->addWidget(logTextEdit);

    stopButton = new QPushButton("停止服务器", this);
    layout->addWidget(stopButton, 0, Qt::AlignRight);

    loadStyleSheet(":/style.qss");

    connect(stopButton, &QPushButton::clicked, this, &Widget::onStopButtonClicked);
    connect(tcpServer, &QTcpServer::newConnection, this, &Widget::onNewConnection);

    // 启动 TCP 服务器，监听 8888 端口
    if (!tcpServer->listen(QHostAddress::Any, 8888)) {
        appendLog(QString("【错误】服务器启动失败：%1").arg(tcpServer->errorString()));
    } else {
        appendLog(QString("【信息】服务器已启动，监听端口 %1").arg(tcpServer->serverPort()));
    }
}

Widget::~Widget()
{
}

void Widget::onNewConnection()
{
    QTcpSocket *clientSocket = tcpServer->nextPendingConnection();
    if (clientSocket) {
        appendLog(QString("【连接】新客户端连接来自 %1:%2")
                      .arg(clientSocket->peerAddress().toString())
                      .arg(clientSocket->peerPort()));

        connect(clientSocket, &QTcpSocket::readyRead, this, &Widget::onReadyRead);
        connect(clientSocket, &QTcpSocket::disconnected, this, &Widget::onClientDisconnected);
        clientNicknames[clientSocket] = "";
    }
}

void Widget::onClientDisconnected()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (clientSocket) {
        QString nickname = clientNicknames.value(clientSocket, "未知用户");

        appendLog(QString("【断开】%1 断开连接").arg(nickname));

        clientNicknames.remove(clientSocket);

        clientSocket->deleteLater();

        QJsonObject leaveMessage;
        leaveMessage["type"] = "user_left";
        leaveMessage["nickname"] = nickname;

        sendMessageToAll(leaveMessage);
    }
}

// --- 槽函数：处理客户端发来的数据 ---
void Widget::onReadyRead()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket)
        return;
    while (clientSocket->canReadLine()) {
        QByteArray jsonData = clientSocket->readLine().trimmed();
        QJsonParseError parseError;

        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            appendLog(QString("【错误】JSON解析失败 (%1): %2")
                          .arg(parseError.errorString())
                          .arg(QString(jsonData)));
            continue;
        }

        if (!jsonDoc.isObject()) {
            appendLog(QString("【警告】收到非JSON对象数据: %1").arg(QString(jsonData)));
            continue;
        }

        QJsonObject obj = jsonDoc.object();
        QString type = obj["type"].toString();

        if (type == "login") {
            QString nickname = obj["nickname"].toString();
            if (!nickname.isEmpty()) {
                if (clientNicknames.values().contains(nickname)) {
                    appendLog(QString("【警告】昵称 '%1' 已存在，拒绝登录").arg(nickname));

                    QJsonObject errorMsg;
                    errorMsg["type"] = "login_failed";
                    errorMsg["reason"] = "昵称已存在";
                    QJsonDocument errorDoc(errorMsg);
                    QByteArray responseData = errorDoc.toJson(QJsonDocument::Compact) + "\n";

                    clientSocket->write(responseData);
                    clientSocket->flush();

                } else {
                    clientNicknames[clientSocket] = nickname;
                    appendLog(QString("【登录】用户 '%1' 登录成功").arg(nickname));

                    QJsonObject successMsg;
                    successMsg["type"] = "login_success";
                    QJsonDocument successDoc(successMsg);

                    QByteArray successData = successDoc.toJson(QJsonDocument::Compact) + "\n";
                    clientSocket->write(successData);
                    clientSocket->flush();

                    QJsonObject joinMessage;
                    joinMessage["type"] = "user_joined";
                    joinMessage["nickname"] = nickname;
                    sendMessageToAll(joinMessage);

                    QJsonObject userListMsg;
                    userListMsg["type"] = "user_list";
                    QJsonArray userArray;
                    for (const QString& name : clientNicknames.values()) {
                        if(!name.isEmpty())
                            userArray.append(name);
                    }
                    userListMsg["users"] = userArray;
                    QJsonDocument listDoc(userListMsg);
                    QByteArray listData = listDoc.toJson(QJsonDocument::Compact) + "\n";
                    clientSocket->write(listData);
                    clientSocket->flush();
                }
            }
        } else if (type == "chat_message") {
            QString message = obj["message"].toString();
            QString senderNickname = clientNicknames.value(clientSocket, "未知用户");
            if (!message.isEmpty()) {
                appendLog(QString("[%1]: %2").arg(senderNickname).arg(message));

                QJsonObject chatMsg;
                chatMsg["type"] = "chat_message";
                chatMsg["sender"] = senderNickname;
                chatMsg["message"] = message;

                sendMessageToAll(chatMsg);
            }
        } else {
            appendLog(QString("【警告】收到未知类型消息: %1").arg(type));
        }
    }
}

void Widget::sendMessageToAll(const QJsonObject &message)
{
    QByteArray data = QJsonDocument(message).toJson(QJsonDocument::Compact) + "\n";

    for (auto it = clientNicknames.constBegin(); it != clientNicknames.constEnd(); ++it) {
        QTcpSocket *socket = it.key();
        if (socket && socket->state() == QAbstractSocket::ConnectedState) {
            socket->write(data);
            socket->flush();
        }
    }
}

void Widget::onStopButtonClicked()
{
    appendLog("【提示】服务器正在停止...");

    tcpServer->close();

    for (auto it = clientNicknames.constBegin(); it != clientNicknames.constEnd(); ++it) {
        QTcpSocket *socket = it.key();
        if (socket) {
            socket->disconnectFromHost();
        }
    }

    appendLog("【提示】服务器已停止。");
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

void Widget::appendLog(const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    logTextEdit->append(QString("[%1] %2").arg(timestamp).arg(message));
}
