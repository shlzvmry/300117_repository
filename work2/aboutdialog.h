#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_pressed();

private:
    Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_H
