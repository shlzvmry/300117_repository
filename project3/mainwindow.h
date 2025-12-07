//mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTableView>
#include <QStandardItemModel>
#include <QSqlTableModel>
#include <QLabel>
#include <QMessageBox>
#include <QGroupBox>
#include <QDateEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 登录页面
    void onLoginClicked();
    void onRegisterClicked();

    // 主页面导航
    void onDepartmentClicked();
    void onDoctorClicked();
    void onPatientClicked();
    void onLogoutClicked();

    // 患者页面
    void onSearchClicked();
    void onAddPatientClicked();
    void onDeletePatientClicked();
    void onEditPatientClicked();
    void onPatientDoubleClicked(const QModelIndex &index);

    // 编辑患者页面
    void onSavePatientClicked();
    void onCancelPatientClicked();

    // 返回按钮
    void onBackClicked();

private:
    void setupUI();
    void loadStyleSheet();

    // 创建页面
    void createLoginPage();
    void createMainPage();
    void createDepartmentPage();
    void createDoctorPage();
    void createPatientPage();
    void createEditPatientPage();

    // 切换页面
    void switchToPage(int index);

    // 刷新数据
    void refreshPatientTable();
    void clearEditPatientForm();
    void loadPatientToForm(const QMap<QString, QVariant> &patient);

    QStackedWidget *stackedWidget;

    // 登录页面组件
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;

    // 患者页面组件
    QLineEdit *searchEdit;
    QTableView *patientTableView;
    QSqlTableModel *patientModel;

    // 编辑患者页面组件
    QLineEdit *editPatientId;
    QLineEdit *editPatientName;
    QLineEdit *editIdCard;
    QComboBox *editSex;
    QDateEdit *editDob;
    QDoubleSpinBox *editHeight;
    QDoubleSpinBox *editWeight;
    QLineEdit *editMobile;
    QSpinBox *editAge;

    // 当前编辑的患者ID
    QString currentEditPatientId;

    // 页面索引常量
    enum PageIndex {
        PAGE_LOGIN = 0,
        PAGE_MAIN,
        PAGE_DEPARTMENT,
        PAGE_DOCTOR,
        PAGE_PATIENT,
        PAGE_EDIT_PATIENT
    };
};

#endif // MAINWINDOW_H
