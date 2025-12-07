//main.cpp
#include "mainwindow.h"
#include "database.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 初始化数据库
    if (!Database::instance().init()) {
        qDebug() << "数据库初始化失败！";
        return -1;
    }

    MainWindow w;
    w.show();
    return a.exec();
}
