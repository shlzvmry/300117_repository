//mainwindow.cpp
#include "mainwindow.h"
#include "database.h"
#include "delegate.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QFile>
#include <QInputDialog>
#include <QScrollArea>
#include <QScreen>
#include <QApplication>
#include <QSqlQuery>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , currentEditPatientId("")
{
    setWindowTitle("医院诊疗测试系统");

    // 获取屏幕尺寸并设置窗口大小为3/4
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int width = screenGeometry.width() * 3 / 4;
    int height = screenGeometry.height() * 3 / 4;
    resize(width, height);

    // 设置最小尺寸，允许缩放
    setMinimumSize(width * 0.6, height * 0.6);

    // 设置开发者信息
    QLabel *developerLabel = new QLabel("开发者: 谢静蕾 2023414300117", this);
    developerLabel->setObjectName("developerLabel");
    developerLabel->setAlignment(Qt::AlignCenter);
    int buttonHeight = 44;
    int buttonPadding = 12;

    // 创建一个悬浮容器
    QWidget *infoWidget = new QWidget(this);
    infoWidget->setObjectName("infoContainer");
    QHBoxLayout *infoLayout = new QHBoxLayout(infoWidget);
    infoLayout->setContentsMargins(15, buttonPadding, 15, buttonPadding);
    infoLayout->addWidget(developerLabel);
    infoWidget->adjustSize();
    int containerWidth = 240;
    int containerHeight = buttonHeight;

    // 设置位置：右上角
    infoWidget->setGeometry(width - containerWidth - 30, 20, containerWidth, containerHeight);
    infoWidget->raise();  // 确保在最上层

    setupUI();
    loadStyleSheet();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    // 创建堆栈窗口
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    // 创建所有页面
    createLoginPage();
    createMainPage();
    createDepartmentPage();
    createDoctorPage();
    createPatientPage();
    createEditPatientPage();

    // 默认显示登录页
    stackedWidget->setCurrentIndex(PAGE_LOGIN);
}

void MainWindow::loadStyleSheet()
{
    QFile file("D:/Qt/Homework/project3/style.qss");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString styleSheet = QString::fromUtf8(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
    }
}

void MainWindow::createLoginPage()
{
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // 顶部弹性空间
    mainLayout->addStretch(2);

    // 标题
    QLabel *titleLabel = new QLabel("欢迎使用诊疗测试系统");
    titleLabel->setObjectName("loginTitle");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel, 0, Qt::AlignCenter);

    // 中间弹性空间
    mainLayout->addStretch(1);

    // 表单容器
    QWidget *formContainer = new QWidget();
    QVBoxLayout *formLayout = new QVBoxLayout(formContainer);
    formLayout->setContentsMargins(50, 30, 50, 30);
    formLayout->setSpacing(25);

    // 用户名
    QHBoxLayout *usernameLayout = new QHBoxLayout();
    QLabel *userLabel = new QLabel("用户名:");
    userLabel->setObjectName("formLabel");
    userLabel->setFixedWidth(80);
    usernameEdit = new QLineEdit();
    usernameEdit->setObjectName("loginInput");
    usernameEdit->setPlaceholderText("请输入用户名");
    usernameEdit->setMinimumHeight(45);

    usernameLayout->addStretch();
    usernameLayout->addWidget(userLabel);
    usernameLayout->addSpacing(15);
    usernameLayout->addWidget(usernameEdit);
    usernameLayout->addStretch();

    // 密码
    QHBoxLayout *passwordLayout = new QHBoxLayout();
    QLabel *pwdLabel = new QLabel("密码:");
    pwdLabel->setObjectName("formLabel");
    pwdLabel->setFixedWidth(80);
    passwordEdit = new QLineEdit();
    passwordEdit->setObjectName("loginInput");
    passwordEdit->setPlaceholderText("请输入密码");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setMinimumHeight(45);

    passwordLayout->addStretch();
    passwordLayout->addWidget(pwdLabel);
    passwordLayout->addSpacing(15);
    passwordLayout->addWidget(passwordEdit);
    passwordLayout->addStretch();

    // 按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *loginBtn = new QPushButton("登录");
    QPushButton *registerBtn = new QPushButton("注册");

    loginBtn->setObjectName("loginButton");
    registerBtn->setObjectName("registerButton");

    loginBtn->setMinimumSize(150, 50);
    registerBtn->setMinimumSize(150, 50);

    connect(loginBtn, &QPushButton::clicked, this, &MainWindow::onLoginClicked);
    connect(registerBtn, &QPushButton::clicked, this, &MainWindow::onRegisterClicked);

    buttonLayout->addStretch();
    buttonLayout->addWidget(loginBtn);
    buttonLayout->addSpacing(30);
    buttonLayout->addWidget(registerBtn);
    buttonLayout->addStretch();

    // 组装表单
    formLayout->addLayout(usernameLayout);
    formLayout->addLayout(passwordLayout);
    formLayout->addSpacing(40);
    formLayout->addLayout(buttonLayout);

    mainLayout->addWidget(formContainer, 0, Qt::AlignCenter);

    // 底部弹性空间
    mainLayout->addStretch(3);

    stackedWidget->addWidget(page);
}

void MainWindow::createMainPage()
{
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    // 标题
    QLabel *titleLabel = new QLabel("诊疗系统管理平台");
    titleLabel->setObjectName("mainTitle");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // 弹性空间
    mainLayout->addStretch(1);

    // 功能按钮区域
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(40);
    gridLayout->setColumnStretch(0, 1);
    gridLayout->setColumnStretch(1, 1);
    gridLayout->setColumnStretch(2, 1);
    gridLayout->setRowStretch(0, 1);
    gridLayout->setRowStretch(1, 0);

    // 创建功能按钮
    QPushButton *deptBtn = new QPushButton("科室管理");
    QPushButton *doctorBtn = new QPushButton("医生管理");
    QPushButton *patientBtn = new QPushButton("患者管理");
    QPushButton *logoutBtn = new QPushButton("退出登录");

    deptBtn->setObjectName("mainButton");
    doctorBtn->setObjectName("mainButton");
    patientBtn->setObjectName("mainButton");
    logoutBtn->setObjectName("logoutButton");

    // 使用最小尺寸
    deptBtn->setMinimumSize(280, 140);
    doctorBtn->setMinimumSize(280, 140);
    patientBtn->setMinimumSize(280, 140);
    logoutBtn->setMinimumSize(180, 50);

    connect(deptBtn, &QPushButton::clicked, this, &MainWindow::onDepartmentClicked);
    connect(doctorBtn, &QPushButton::clicked, this, &MainWindow::onDoctorClicked);
    connect(patientBtn, &QPushButton::clicked, this, &MainWindow::onPatientClicked);
    connect(logoutBtn, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);

    gridLayout->addWidget(deptBtn, 0, 0, Qt::AlignCenter);
    gridLayout->addWidget(doctorBtn, 0, 1, Qt::AlignCenter);
    gridLayout->addWidget(patientBtn, 0, 2, Qt::AlignCenter);
    gridLayout->addWidget(logoutBtn, 1, 1, Qt::AlignCenter);

    mainLayout->addLayout(gridLayout, 1);

    // 弹性空间
    mainLayout->addStretch(1);

    stackedWidget->addWidget(page);
}

void MainWindow::createDepartmentPage()
{
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setContentsMargins(30, 20, 30, 20);
    mainLayout->setSpacing(20);

    QHBoxLayout *headerLayout = new QHBoxLayout();
    QPushButton *backBtn = new QPushButton("← 返回");
    backBtn->setObjectName("backButton");
    connect(backBtn, &QPushButton::clicked, this, &MainWindow::onBackClicked);

    QLabel *titleLabel = new QLabel("科室信息管理");
    titleLabel->setObjectName("pageTitle");

    headerLayout->addWidget(backBtn);
    headerLayout->addStretch();
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();

    // 科室表格
    QTableView *tableView = new QTableView();
    tableView->setObjectName("dataTable");
    QStandardItemModel *model = new QStandardItemModel(this);

    model->setHorizontalHeaderLabels({"科室编号", "科室名称", "医生数量"});

    // 从数据库获取科室数据
    auto departments = Database::instance().getDepartments();

    // 统计每个科室的医生数量（从数据库查询）
    QMap<QString, int> doctorCount;
    QSqlQuery countQuery("SELECT DEPARTMENT_ID, COUNT(*) as count FROM Doctor GROUP BY DEPARTMENT_ID");
    while (countQuery.next()) {
        QString deptId = countQuery.value("DEPARTMENT_ID").toString();
        int count = countQuery.value("count").toInt();
        doctorCount[deptId] = count;
    }

    // 填充数据
    for (const auto &dept : departments) {
        QList<QStandardItem*> row;
        row.append(new QStandardItem(dept["ID"]));
        row.append(new QStandardItem(dept["NAME"]));
        row.append(new QStandardItem(QString::number(doctorCount.value(dept["ID"], 0))));
        model->appendRow(row);
    }

    tableView->setModel(model);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    tableView->horizontalHeader()->setStretchLastSection(true);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 设置列宽
    tableView->setColumnWidth(0, 150);
    tableView->setColumnWidth(1, 250);
    tableView->setColumnWidth(2, 150);

    mainLayout->addLayout(headerLayout);
    mainLayout->addWidget(tableView, 1);

    stackedWidget->addWidget(page);
}

void MainWindow::createDoctorPage()
{
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setContentsMargins(30, 20, 30, 20);
    mainLayout->setSpacing(20);

    // 返回按钮和标题在同一行
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QPushButton *backBtn = new QPushButton("← 返回");
    backBtn->setObjectName("backButton");
    connect(backBtn, &QPushButton::clicked, this, &MainWindow::onBackClicked);

    QLabel *titleLabel = new QLabel("医生信息管理");
    titleLabel->setObjectName("pageTitle");

    headerLayout->addWidget(backBtn);
    headerLayout->addStretch();
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();

    // 医生表格
    QTableView *tableView = new QTableView();
    tableView->setObjectName("dataTable");
    QStandardItemModel *model = new QStandardItemModel(this);

    model->setHorizontalHeaderLabels({"医生编号", "工号", "姓名", "所属科室"});

    // 从数据库获取数据
    auto doctors = Database::instance().getDoctors();

    // 建立科室ID到名称的映射（从数据库查询）
    QMap<QString, QString> deptMap;
    QSqlQuery deptQuery("SELECT ID, NAME FROM Department");
    while (deptQuery.next()) {
        deptMap[deptQuery.value("ID").toString()] = deptQuery.value("NAME").toString();
    }

    // 填充数据
    for (const auto &doctor : doctors) {
        QList<QStandardItem*> row;
        row.append(new QStandardItem(doctor["ID"]));
        row.append(new QStandardItem(doctor["EMPLOYEENO"]));
        row.append(new QStandardItem(doctor["NAME"]));
        row.append(new QStandardItem(deptMap.value(doctor["DEPARTMENT_ID"], "未知科室")));
        model->appendRow(row);
    }

    tableView->setModel(model);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    tableView->horizontalHeader()->setStretchLastSection(true);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 设置列宽
    tableView->setColumnWidth(0, 150);
    tableView->setColumnWidth(1, 150);
    tableView->setColumnWidth(2, 150);
    tableView->setColumnWidth(3, 200);

    mainLayout->addLayout(headerLayout);
    mainLayout->addWidget(tableView, 1);

    stackedWidget->addWidget(page);
}

void MainWindow::createPatientPage()
{
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setContentsMargins(30, 20, 30, 20);
    mainLayout->setSpacing(20);

    QHBoxLayout *headerLayout = new QHBoxLayout();
    QPushButton *backBtn = new QPushButton("← 返回");
    backBtn->setObjectName("backButton");
    connect(backBtn, &QPushButton::clicked, this, &MainWindow::onBackClicked);

    QLabel *titleLabel = new QLabel("患者信息管理");
    titleLabel->setObjectName("pageTitle");

    headerLayout->addWidget(backBtn);
    headerLayout->addStretch();
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();

    // 搜索区域
    QWidget *searchWidget = new QWidget();
    QHBoxLayout *searchLayout = new QHBoxLayout(searchWidget);
    searchLayout->setSpacing(15);

    searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText("输入患者ID、姓名、身份证号或手机号搜索...");
    searchEdit->setObjectName("searchInput");
    searchEdit->setMinimumHeight(45);

    QPushButton *searchBtn = new QPushButton("搜索");
    QPushButton *addBtn = new QPushButton("添加");
    QPushButton *editBtn = new QPushButton("编辑");
    QPushButton *deleteBtn = new QPushButton("删除");

    searchBtn->setObjectName("searchButton");
    addBtn->setObjectName("actionButton");
    editBtn->setObjectName("actionButton");
    deleteBtn->setObjectName("actionButton");

    int buttonHeight = 45;
    searchBtn->setMinimumSize(100, buttonHeight);
    addBtn->setMinimumSize(100, buttonHeight);
    editBtn->setMinimumSize(100, buttonHeight);
    deleteBtn->setMinimumSize(100, buttonHeight);

    connect(searchBtn, &QPushButton::clicked, this, &MainWindow::onSearchClicked);
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddPatientClicked);
    connect(editBtn, &QPushButton::clicked, this, &MainWindow::onEditPatientClicked);
    connect(deleteBtn, &QPushButton::clicked, this, &MainWindow::onDeletePatientClicked);

    searchLayout->addWidget(searchEdit, 1);
    searchLayout->addWidget(searchBtn);
    searchLayout->addWidget(addBtn);
    searchLayout->addWidget(editBtn);
    searchLayout->addWidget(deleteBtn);

    // 患者表格
    patientTableView = new QTableView();
    patientTableView->setObjectName("dataTable");
    patientModel = new QSqlTableModel(this, Database::instance().getDatabase());
    patientModel->setTable("Patient");
    patientModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    // 设置列名
    patientModel->setHeaderData(0, Qt::Horizontal, "患者ID");
    patientModel->setHeaderData(1, Qt::Horizontal, "身份证号");
    patientModel->setHeaderData(2, Qt::Horizontal, "姓名");
    patientModel->setHeaderData(3, Qt::Horizontal, "性别");
    patientModel->setHeaderData(4, Qt::Horizontal, "出生日期");
    patientModel->setHeaderData(5, Qt::Horizontal, "身高(cm)");
    patientModel->setHeaderData(6, Qt::Horizontal, "体重(kg)");
    patientModel->setHeaderData(7, Qt::Horizontal, "手机号");
    patientModel->setHeaderData(8, Qt::Horizontal, "年龄");
    patientModel->setHeaderData(9, Qt::Horizontal, "创建时间");

    // 设置自定义代理
    patientTableView->setItemDelegateForColumn(3, new SexDelegate(this));
    patientTableView->setItemDelegateForColumn(4, new DateDelegate(this));
    patientTableView->setItemDelegateForColumn(5, new NumberDelegate(this));
    patientTableView->setItemDelegateForColumn(6, new NumberDelegate(this));

    patientTableView->setModel(patientModel);
    patientTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    patientTableView->horizontalHeader()->setStretchLastSection(false);

    // 设置初始列宽（可以拖动查看完整内容）
    patientTableView->setColumnWidth(0, 120);  // ID
    patientTableView->setColumnWidth(1, 180);  // 身份证
    patientTableView->setColumnWidth(2, 120);  // 姓名
    patientTableView->setColumnWidth(3, 80);   // 性别
    patientTableView->setColumnWidth(4, 120);  // 出生日期
    patientTableView->setColumnWidth(5, 100);  // 身高
    patientTableView->setColumnWidth(6, 100);  // 体重
    patientTableView->setColumnWidth(7, 140);  // 手机号
    patientTableView->setColumnWidth(8, 80);   // 年龄
    patientTableView->setColumnWidth(9, 160);  // 创建时间

    connect(patientTableView, &QTableView::doubleClicked,
            this, &MainWindow::onPatientDoubleClicked);

    refreshPatientTable();

    mainLayout->addLayout(headerLayout);
    mainLayout->addWidget(searchWidget);
    mainLayout->addWidget(patientTableView, 1);

    stackedWidget->addWidget(page);
}

void MainWindow::createEditPatientPage()
{
    // 使用滚动区域容纳表单
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setContentsMargins(40, 30, 40, 30);
    mainLayout->setSpacing(25);

    // 返回按钮和标题
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QPushButton *backBtn = new QPushButton("← 返回");
    backBtn->setObjectName("backButton");
    backBtn->setMinimumSize(100, 40);
    connect(backBtn, &QPushButton::clicked, this, &MainWindow::onBackClicked);

    QLabel *titleLabel = new QLabel("编辑患者信息");
    titleLabel->setObjectName("pageTitle");

    headerLayout->addWidget(backBtn);
    headerLayout->addStretch();
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();

    // 表单容器
    QWidget *formContainer = new QWidget();
    QVBoxLayout *formLayout = new QVBoxLayout(formContainer);
    formLayout->setContentsMargins(50, 30, 50, 30);
    formLayout->setSpacing(30);

    // 基本信息组
    QGroupBox *basicGroup = new QGroupBox("基本信息");
    basicGroup->setObjectName("formGroup");
    QGridLayout *basicLayout = new QGridLayout(basicGroup);
    basicLayout->setHorizontalSpacing(30);
    basicLayout->setVerticalSpacing(20);
    basicLayout->setColumnStretch(1, 1);

    // 患者ID（只读，新增时自动生成）
    basicLayout->addWidget(new QLabel("患者ID:"), 0, 0);
    editPatientId = new QLineEdit();
    editPatientId->setReadOnly(true);
    editPatientId->setObjectName("formInput");
    editPatientId->setMinimumHeight(45);
    basicLayout->addWidget(editPatientId, 0, 1);

    basicLayout->addWidget(new QLabel("姓名*:"), 1, 0);
    editPatientName = new QLineEdit();
    editPatientName->setObjectName("formInput");
    editPatientName->setMinimumHeight(45);
    basicLayout->addWidget(editPatientName, 1, 1);

    basicLayout->addWidget(new QLabel("身份证号:"), 2, 0);
    editIdCard = new QLineEdit();
    editIdCard->setObjectName("formInput");
    editIdCard->setMinimumHeight(45);
    basicLayout->addWidget(editIdCard, 2, 1);

    basicLayout->addWidget(new QLabel("性别:"), 3, 0);
    editSex = new QComboBox();
    editSex->addItem("男", 1);
    editSex->addItem("女", 0);
    editSex->setObjectName("formCombo");
    editSex->setMinimumHeight(45);
    basicLayout->addWidget(editSex, 3, 1);

    // 详细信息组
    QGroupBox *detailGroup = new QGroupBox("详细信息");
    detailGroup->setObjectName("formGroup");
    QGridLayout *detailLayout = new QGridLayout(detailGroup);
    detailLayout->setHorizontalSpacing(30);
    detailLayout->setVerticalSpacing(20);
    detailLayout->setColumnStretch(1, 1);

    detailLayout->addWidget(new QLabel("出生日期:"), 0, 0);
    editDob = new QDateEdit();
    editDob->setCalendarPopup(true);
    editDob->setDisplayFormat("yyyy-MM-dd");
    editDob->setDate(QDate::currentDate().addYears(-30));
    editDob->setObjectName("formDate");
    editDob->setMinimumHeight(45);
    detailLayout->addWidget(editDob, 0, 1);

    detailLayout->addWidget(new QLabel("身高(cm):"), 1, 0);
    editHeight = new QDoubleSpinBox();
    editHeight->setMinimum(0);
    editHeight->setMaximum(300);
    editHeight->setSingleStep(0.1);
    editHeight->setValue(0);
    editHeight->setObjectName("formSpin");
    editHeight->setMinimumHeight(45);
    detailLayout->addWidget(editHeight, 1, 1);

    detailLayout->addWidget(new QLabel("体重(kg):"), 2, 0);
    editWeight = new QDoubleSpinBox();
    editWeight->setMinimum(0);
    editWeight->setMaximum(300);
    editWeight->setSingleStep(0.1);
    editWeight->setValue(0);
    editWeight->setObjectName("formSpin");
    editWeight->setMinimumHeight(45);
    detailLayout->addWidget(editWeight, 2, 1);

    detailLayout->addWidget(new QLabel("手机号:"), 3, 0);
    editMobile = new QLineEdit();
    editMobile->setObjectName("formInput");
    editMobile->setMinimumHeight(45);
    detailLayout->addWidget(editMobile, 3, 1);

    detailLayout->addWidget(new QLabel("年龄:"), 4, 0);
    editAge = new QSpinBox();
    editAge->setMinimum(0);
    editAge->setMaximum(150);
    editAge->setValue(0);
    editAge->setObjectName("formSpin");
    editAge->setMinimumHeight(45);
    editAge->setReadOnly(true);  // 设置为只读
    editAge->setButtonSymbols(QSpinBox::NoButtons);  // 隐藏调节按钮
    editAge->setStyleSheet("background-color: #3a4a5a; color: #a0b0c0;");
    detailLayout->addWidget(editAge, 4, 1);

    // 连接生日变化信号，自动计算年龄
    connect(editDob, &QDateEdit::dateChanged, this, [=](const QDate &date) {
        if (date.isValid()) {
            QDate currentDate = QDate::currentDate();
            int age = currentDate.year() - date.year();

            // 如果今年还没过生日，年龄减1
            if (currentDate.month() < date.month() ||
                (currentDate.month() == date.month() && currentDate.day() < date.day())) {
                age--;
            }

            if (age < 0) age = 0;  // 确保年龄不为负
            editAge->setValue(age);
        }
    });

    editSex->setStyleSheet(
        "QComboBox::down-arrow {"
        "    image: url(:/icons/arrow-down-s-line.png);"
        "    width: 24px;"
        "    height: 24px;"
        "}"
        "QComboBox::down-arrow:on {"
        "    image: url(:/icons/arrow-down-s-line.png);"
        "}"
        );

    editDob->setStyleSheet(
        "QDateEdit::down-arrow {"
        "    image: url(:/icons/calendar-line.png);"
        "    width: 20px;"
        "    height :24px;"
        "}"
        );

    editHeight->setStyleSheet(
        "QDoubleSpinBox::up-arrow {"
        "    image: url(:/icons/add-line.png);"
        "    width: 24px;"
        "    height: 24px;"
        "}"
        "QDoubleSpinBox::down-arrow {"
        "    image: url(:/icons/subtract-line.png);"
        "    width: 24px;"
        "    height: 24px;"
        "}"
        );

    editWeight->setStyleSheet(
        "QDoubleSpinBox::up-arrow {"
        "    image: url(:/icons/add-line.png);"
        "    width: 24px;"
        "    height: 24px;"
        "}"
        "QDoubleSpinBox::down-arrow {"
        "    image: url(:/icons/subtract-line.png);"
        "    width: 24px;"
        "    height: 24px;"
        "}"
        );

    editAge->setStyleSheet(
        "QSpinBox::up-arrow {"
        "    image: url(:/icons/add-line.png);"
        "    width: 24px;"
        "    height: 24px;"
        "}"
        "QSpinBox::down-arrow {"
        "    image: url(:/icons/subtract-line.png);"
        "    width: 24px;"
        "    height: 24px;"
        "}"
        );

    // 按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *saveBtn = new QPushButton("保存");
    QPushButton *cancelBtn = new QPushButton("取消");

    saveBtn->setObjectName("primaryButton");
    cancelBtn->setObjectName("secondaryButton");

    saveBtn->setMinimumSize(150, 50);
    cancelBtn->setMinimumSize(150, 50);

    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::onSavePatientClicked);
    connect(cancelBtn, &QPushButton::clicked, this, &MainWindow::onCancelPatientClicked);

    buttonLayout->addStretch();
    buttonLayout->addWidget(saveBtn);
    buttonLayout->addSpacing(40);
    buttonLayout->addWidget(cancelBtn);
    buttonLayout->addStretch();

    // 组装表单
    formLayout->addWidget(basicGroup);
    formLayout->addWidget(detailGroup);
    formLayout->addSpacing(40);
    formLayout->addLayout(buttonLayout);

    // 组装主布局
    mainLayout->addLayout(headerLayout);
    mainLayout->addWidget(formContainer);

    scrollArea->setWidget(page);

    stackedWidget->addWidget(scrollArea);
}

//页面切换函数
void MainWindow::switchToPage(int index)
{
    if (index >= 0 && index < stackedWidget->count()) {
        stackedWidget->setCurrentIndex(index);
    }
}

// 槽函数实现
void MainWindow::onLoginClicked()
{
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text().trimmed();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请输入用户名和密码");
        return;
    }

    if (Database::instance().login(username, password)) {
        // 清空登录框
        usernameEdit->clear();
        passwordEdit->clear();

        // 切换到主页面
        switchToPage(PAGE_MAIN);
    } else {
        QMessageBox::warning(this, "登录失败", "用户名或密码错误");
    }
}

void MainWindow::onRegisterClicked()
{
    // 第一步：输入真实姓名
    QString fullname = QInputDialog::getText(this, "用户注册",
                                             "请输入真实姓名:",
                                             QLineEdit::Normal, "", nullptr);

    if (fullname.isEmpty()) {
        // 用户取消或输入为空
        return;
    }

    // 检查真实姓名是否已存在
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM User WHERE FULLNAME = ?");
    checkQuery.addBindValue(fullname);

    if (checkQuery.exec() && checkQuery.next()) {
        int count = checkQuery.value(0).toInt();
        if (count > 0) {
            QMessageBox::warning(this, "注册失败",
                                 QString("真实姓名 '%1' 已被使用，请使用其他姓名或联系管理员")
                                     .arg(fullname));
            return;
        }
    }

    // 第二步：创建账号信息对话框
    QDialog dialog(this);
    dialog.setWindowTitle("设置账号信息");
    dialog.setObjectName("registerDialog");
    dialog.setMinimumWidth(400);

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setContentsMargins(25, 25, 25, 25);
    mainLayout->setSpacing(15);

    // 用户名输入
    QLabel *userLabel = new QLabel("用户名:");
    userLabel->setObjectName("dialogLabel");
    QLineEdit *userEdit = new QLineEdit();
    userEdit->setObjectName("dialogInput");
    userEdit->setPlaceholderText("至少3个字符");

    // 密码输入
    QLabel *pwdLabel = new QLabel("密码:");
    pwdLabel->setObjectName("dialogLabel");
    QLineEdit *pwdEdit = new QLineEdit();
    pwdEdit->setObjectName("dialogInput");
    pwdEdit->setPlaceholderText("至少6个字符");
    pwdEdit->setEchoMode(QLineEdit::Password);

    // 确认密码
    QLabel *confirmLabel = new QLabel("确认密码:");
    confirmLabel->setObjectName("dialogLabel");
    QLineEdit *confirmEdit = new QLineEdit();
    confirmEdit->setObjectName("dialogInput");
    confirmEdit->setPlaceholderText("再次输入密码");
    confirmEdit->setEchoMode(QLineEdit::Password);

    // 按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("确认注册");
    QPushButton *cancelButton = new QPushButton("取消");

    okButton->setObjectName("dialogOkButton");
    cancelButton->setObjectName("dialogCancelButton");

    okButton->setMinimumSize(100, 40);
    cancelButton->setMinimumSize(100, 40);

    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addSpacing(15);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addStretch();

    // 组装布局
    mainLayout->addWidget(userLabel);
    mainLayout->addWidget(userEdit);
    mainLayout->addWidget(pwdLabel);
    mainLayout->addWidget(pwdEdit);
    mainLayout->addWidget(confirmLabel);
    mainLayout->addWidget(confirmEdit);
    mainLayout->addSpacing(25);
    mainLayout->addLayout(buttonLayout);

    // 连接按钮
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
    connect(okButton, &QPushButton::clicked, [&]() {
        QString username = userEdit->text().trimmed();
        QString password = pwdEdit->text().trimmed();
        QString confirmPassword = confirmEdit->text().trimmed();

        // 验证输入
        if (username.length() < 3) {
            QMessageBox::warning(this, "输入错误", "用户名至少需要3个字符");
            userEdit->setFocus();
            return;
        }

        if (password.length() < 6) {
            QMessageBox::warning(this, "输入错误", "密码至少需要6个字符");
            pwdEdit->setFocus();
            return;
        }

        if (password != confirmPassword) {
            QMessageBox::warning(this, "输入错误", "两次输入的密码不一致");
            confirmEdit->clear();
            confirmEdit->setFocus();
            return;
        }

        // 尝试注册
        if (Database::instance().registerUser(fullname, username, password)) {
            QMessageBox::information(this, "注册成功", "用户注册成功！");
            dialog.accept();
        } else {
            QMessageBox::warning(this, "注册失败", "用户名可能已存在，请更换用户名");
            userEdit->clear();
            userEdit->setFocus();
        }
    });

    // 显示对话框
    if (dialog.exec() == QDialog::Accepted) {
        // 注册成功，清空登录输入框
        usernameEdit->clear();
        passwordEdit->clear();
    }
}

void MainWindow::onLogoutClicked()
{
    // 切换到登录页面
    switchToPage(PAGE_LOGIN);
}

void MainWindow::onDepartmentClicked()
{
    switchToPage(PAGE_DEPARTMENT);
}

void MainWindow::onDoctorClicked()
{
    switchToPage(PAGE_DOCTOR);
}

void MainWindow::onPatientClicked()
{
    refreshPatientTable();
    switchToPage(PAGE_PATIENT);
}

void MainWindow::onBackClicked()
{
    int currentIndex = stackedWidget->currentIndex();

    if (currentIndex == PAGE_MAIN) {
        switchToPage(PAGE_LOGIN);
    } else if (currentIndex >= PAGE_DEPARTMENT && currentIndex <= PAGE_EDIT_PATIENT) {
        switchToPage(PAGE_MAIN);
    }
}

void MainWindow::onSearchClicked()
{
    QString filter = searchEdit->text().trimmed();

    if (!filter.isEmpty()) {
        QString whereClause = QString(
                                  "ID LIKE '%%1%' OR "
                                  "NAME LIKE '%%1%' OR "
                                  "ID_CARD LIKE '%%1%' OR "
                                  "MOBILEPHONE LIKE '%%1%'"
                                  ).arg(filter);

        patientModel->setFilter(whereClause);
    } else {
        patientModel->setFilter("");
    }

    refreshPatientTable();
}

void MainWindow::onAddPatientClicked()
{
    clearEditPatientForm();
    editPatientId->setText("新增患者（保存时自动生成）");
    currentEditPatientId = "";
    switchToPage(PAGE_EDIT_PATIENT);
}

void MainWindow::onEditPatientClicked()
{
    QModelIndexList selected = patientTableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "选择错误", "请先选择要编辑的患者");
        return;
    }

    int row = selected.first().row();
    QMap<QString, QVariant> patient;
    patient["ID"] = patientModel->data(patientModel->index(row, 0));
    patient["ID_CARD"] = patientModel->data(patientModel->index(row, 1));
    patient["NAME"] = patientModel->data(patientModel->index(row, 2));
    patient["SEX"] = patientModel->data(patientModel->index(row, 3));
    patient["DOB"] = patientModel->data(patientModel->index(row, 4));
    patient["HEIGHT"] = patientModel->data(patientModel->index(row, 5));
    patient["WEIGHT"] = patientModel->data(patientModel->index(row, 6));
    patient["MOBILEPHONE"] = patientModel->data(patientModel->index(row, 7));
    patient["AGE"] = patientModel->data(patientModel->index(row, 8));

    loadPatientToForm(patient);
    switchToPage(PAGE_EDIT_PATIENT);
}

void MainWindow::onDeletePatientClicked()
{
    QModelIndexList selected = patientTableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "选择错误", "请先选择要删除的患者");
        return;
    }

    int result = QMessageBox::question(this, "确认删除",
                                       "确定要删除选中的患者吗？此操作不可恢复。",
                                       QMessageBox::Yes | QMessageBox::No);

    if (result == QMessageBox::Yes) {
        for (const QModelIndex &index : selected) {
            int row = index.row();
            QString id = patientModel->data(patientModel->index(row, 0)).toString();
            Database::instance().deletePatient(id);
        }
        refreshPatientTable();
    }
}

void MainWindow::onPatientDoubleClicked(const QModelIndex &index)
{
    int row = index.row();
    QMap<QString, QVariant> patient;
    patient["ID"] = patientModel->data(patientModel->index(row, 0));
    patient["ID_CARD"] = patientModel->data(patientModel->index(row, 1));
    patient["NAME"] = patientModel->data(patientModel->index(row, 2));
    patient["SEX"] = patientModel->data(patientModel->index(row, 3));
    patient["DOB"] = patientModel->data(patientModel->index(row, 4));
    patient["HEIGHT"] = patientModel->data(patientModel->index(row, 5));
    patient["WEIGHT"] = patientModel->data(patientModel->index(row, 6));
    patient["MOBILEPHONE"] = patientModel->data(patientModel->index(row, 7));
    patient["AGE"] = patientModel->data(patientModel->index(row, 8));

    loadPatientToForm(patient);
    switchToPage(PAGE_EDIT_PATIENT);
}

void MainWindow::onSavePatientClicked()
{
    // 验证必填项
    if (editPatientName->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "验证错误", "患者姓名不能为空");
        editPatientName->setFocus();
        return;
    }

    QMap<QString, QVariant> patient;
    patient["ID_CARD"] = editIdCard->text().trimmed();
    patient["NAME"] = editPatientName->text().trimmed();
    patient["SEX"] = editSex->currentData().toInt();
    patient["DOB"] = editDob->date().toString("yyyy-MM-dd");
    patient["HEIGHT"] = editHeight->value();
    patient["WEIGHT"] = editWeight->value();
    patient["MOBILEPHONE"] = editMobile->text().trimmed();
    patient["AGE"] = editAge->value();

    bool success = false;

    if (currentEditPatientId.isEmpty()) {
        // 新增患者
        success = Database::instance().addPatient(patient);
    } else {
        // 更新患者
        success = Database::instance().updatePatient(currentEditPatientId, patient);
    }

    if (success) {
        QMessageBox::information(this, "操作成功",
                                 currentEditPatientId.isEmpty() ?
                                     "患者添加成功" : "患者信息更新成功");
        refreshPatientTable();
        switchToPage(PAGE_PATIENT);
    } else {
        QMessageBox::warning(this, "操作失败",
                             "保存患者信息失败，请检查数据格式");
    }
}

void MainWindow::onCancelPatientClicked()
{
    switchToPage(PAGE_PATIENT);
}

// 辅助函数
void MainWindow::refreshPatientTable()
{
    patientModel->select();
    patientTableView->resizeColumnsToContents();
    patientTableView->setColumnWidth(2, 85);   // 姓名
    patientTableView->setColumnWidth(5, 85);   // 身高
    patientTableView->setColumnWidth(6, 80);   // 体重
}

void MainWindow::clearEditPatientForm()
{
    editPatientId->clear();
    editPatientName->clear();
    editIdCard->clear();
    editSex->setCurrentIndex(0);
    editDob->setDate(QDate::currentDate().addYears(-30));
    editHeight->setValue(0);
    editWeight->setValue(0);
    editMobile->clear();
    editAge->setValue(0);
    currentEditPatientId = "";
}

void MainWindow::loadPatientToForm(const QMap<QString, QVariant> &patient)
{
    currentEditPatientId = patient["ID"].toString();
    editPatientId->setText(currentEditPatientId);
    editPatientName->setText(patient["NAME"].toString());
    editIdCard->setText(patient["ID_CARD"].toString());
    editSex->setCurrentIndex(patient["SEX"].toInt() == 1 ? 0 : 1);

    QDate dob = QDate::fromString(patient["DOB"].toString(), "yyyy-MM-dd");
    if (dob.isValid()) {
        editDob->setDate(dob);
    }

    editHeight->setValue(patient["HEIGHT"].toDouble());
    editWeight->setValue(patient["WEIGHT"].toDouble());
    editMobile->setText(patient["MOBILEPHONE"].toString());
    editAge->setValue(patient["AGE"].toInt());
}
