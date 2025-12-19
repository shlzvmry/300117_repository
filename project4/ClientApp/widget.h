#ifndef WIDGET_H
#define WIDGET_H

#include <QMainWindow>
#include <QStringList>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>

// 前向声明
class QStackedWidget;
class QWidget;
class QLineEdit;
class QPushButton;
class QTextEdit;
class QListWidget;

class Widget : public QMainWindow
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void onLoginButtonClicked();
    void onSendButtonClicked();
    void onExitButtonClicked();

    // 网络相关槽函数
    void onConnected();
    void onDisconnected();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);
    void onReadyRead();

private:
    void createLoginInterface();
    void createChatInterface();
    void loadStyleSheet(const QString &sheetName);
    void updateUserList(const QStringList &users);

    // 界面组件指针
    QStackedWidget *stackedWidget;
    QWidget *loginWidget;
    QWidget *chatWidget;

    // 登录界面控件
    QLineEdit *ipLineEdit;
    QLineEdit *nicknameLineEdit;
    QPushButton *loginButton;

    // 聊天界面控件
    QTextEdit *chatTextEdit;
    QLineEdit *inputLineEdit;
    QPushButton *sendButton;
    QPushButton *exitButton;
    QListWidget *userListWidget;

    // 网络相关
    QTcpSocket *tcpSocket;
    QString myNickname;
    void appendChatMessage(const QString &message);
    void handleServerMessage(const QJsonObject &obj);
};
#endif // WIDGET_H
