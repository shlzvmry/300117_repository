#ifndef WIDGET_H
#define WIDGET_H

#include <QMainWindow>
#include <QTcpServer>

QT_BEGIN_NAMESPACE
class QLabel;
class QTextEdit;
class QPushButton;
class QTcpSocket;
QT_END_NAMESPACE

class Widget : public QMainWindow
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void onNewConnection();
    void onClientDisconnected();
    void onReadyRead();
    void onStopButtonClicked();

private:
    QTcpServer *tcpServer;
    QTextEdit *logTextEdit;
    QLabel *infoLabel;
    QPushButton *stopButton;

    QMap<QTcpSocket*, QString> clientNicknames;

    void loadStyleSheet(const QString &sheetName);
    void appendLog(const QString &message);
    void sendMessageToAll(const QJsonObject &message);
};

#endif
