//mian.cpp
#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 设置应用程序信息
    QApplication::setApplicationName("文本编辑器");
    QApplication::setApplicationVersion("1.0");
    QApplication::setOrganizationName("MyCompany");

    MainWindow window;
    window.show();

    return app.exec();
}
