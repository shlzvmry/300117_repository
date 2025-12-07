//database.cpp
#include "database.h"

Database& Database::instance()
{
    static Database instance;
    return instance;
}

Database::Database(QObject *parent) : QObject(parent)
{
}

bool Database::init()
{
    // 使用绝对路径连接数据库
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("D:/Qt/Homework/project3/HospitalDB.db");

    qDebug() << "正在连接数据库:" << db.databaseName();

    if (!db.open()) {
        qDebug() << "无法打开数据库:" << db.lastError().text();
        return false;
    }

    qDebug() << "数据库连接成功！";
    return true;
}

bool Database::login(const QString &username, const QString &password)
{
    QSqlQuery query;
    query.prepare("SELECT ID, FULLNAME FROM User WHERE USERNAME = ? AND PASSWORD = ?");
    query.addBindValue(username);
    query.addBindValue(password);

    if (query.exec() && query.next()) {
        currentUserId = query.value("ID").toString();
        qDebug() << "用户登录成功:" << username;
        addHistory("用户登录");
        return true;
    }
    qDebug() << "登录失败，用户名:" << username;
    return false;
}

bool Database::registerUser(const QString &fullname, const QString &username, const QString &password)
{
    // 首先检查用户名是否已存在
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM User WHERE USERNAME = ?");
    checkQuery.addBindValue(username);

    if (checkQuery.exec() && checkQuery.next() && checkQuery.value(0).toInt() > 0) {
        qDebug() << "用户名已存在:" << username;
        return false;
    }

    // 获取最大ID值，生成新ID
    QSqlQuery maxIdQuery("SELECT MAX(CAST(REPLACE(ID, 'ID_', '') AS INTEGER)) FROM User");
    int nextId = 1;
    if (maxIdQuery.exec() && maxIdQuery.next()) {
        int maxId = maxIdQuery.value(0).toInt();
        nextId = maxId + 1;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO User (ID, FULLNAME, USERNAME, PASSWORD) VALUES (?, ?, ?, ?)");
    query.addBindValue(QString::number(nextId));  // 使用数字ID
    query.addBindValue(fullname);
    query.addBindValue(username);
    query.addBindValue(password);

    if (query.exec()) {
        qDebug() << "用户注册成功: ID=" << nextId << ", USERNAME=" << username;
        addHistory("注册新用户: " + username);
        return true;
    }
    qDebug() << "用户注册失败:" << query.lastError().text();
    return false;
}

QList<QMap<QString, QVariant>> Database::getPatients(const QString &filter)
{
    QList<QMap<QString, QVariant>> patients;
    QString sql = "SELECT * FROM Patient";

    if (!filter.isEmpty()) {
        sql += " WHERE NAME LIKE '%" + filter + "%' OR ID_CARD LIKE '%" + filter + "%' OR MOBILEPHONE LIKE '%" + filter + "%'";
    }

    QSqlQuery query(sql);
    while (query.next()) {
        QMap<QString, QVariant> patient;
        patient["ID"] = query.value("ID");
        patient["ID_CARD"] = query.value("ID_CARD");
        patient["NAME"] = query.value("NAME");
        patient["SEX"] = query.value("SEX");
        patient["DOB"] = query.value("DOB");
        patient["HEIGHT"] = query.value("HEIGHT");
        patient["WEIGHT"] = query.value("WEIGHT");
        patient["MOBILEPHONE"] = query.value("MOBILEPHONE");
        patient["AGE"] = query.value("AGE");
        patient["CREATEDTIMESTAMP"] = query.value("CREATEDTIMESTAMP");

        patients.append(patient);
    }

    qDebug() << "获取患者数量:" << patients.size();
    return patients;
}

bool Database::addPatient(const QMap<QString, QVariant> &patient)
{
    // 1. 生成按顺序的ID
    QSqlQuery maxIdQuery("SELECT MAX(ID) FROM Patient WHERE ID LIKE 'P%'");
    int nextIdNum = 1;

    if (maxIdQuery.exec() && maxIdQuery.next()) {
        QString maxId = maxIdQuery.value(0).toString();
        if (!maxId.isEmpty() && maxId.startsWith('P')) {
            // 提取数字部分
            QString numStr = maxId.mid(1);  // 去掉开头的'P'
            bool ok;
            int maxNum = numStr.toInt(&ok);
            if (ok && maxNum > 0) {
                nextIdNum = maxNum + 1;
            }
        }
    }

    QString newId = QString("P%1").arg(nextIdNum, 3, 10, QChar('0'));  // P001格式

    // 2. 自动计算年龄
    QString dobStr = patient["DOB"].toString();
    int age = 0;
    if (!dobStr.isEmpty()) {
        QDate dob = QDate::fromString(dobStr, "yyyy-MM-dd");
        if (dob.isValid()) {
            QDate currentDate = QDate::currentDate();
            age = currentDate.year() - dob.year();

            // 如果今年还没过生日，年龄减1
            if (currentDate.month() < dob.month() ||
                (currentDate.month() == dob.month() && currentDate.day() < dob.day())) {
                age--;
            }
        }
    }

    QSqlQuery query;
    query.prepare("INSERT INTO Patient (ID, ID_CARD, NAME, SEX, DOB, HEIGHT, WEIGHT, MOBILEPHONE, AGE, CREATEDTIMESTAMP) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

    query.addBindValue(newId);
    query.addBindValue(patient["ID_CARD"]);
    query.addBindValue(patient["NAME"]);
    query.addBindValue(patient["SEX"]);
    query.addBindValue(patient["DOB"]);
    query.addBindValue(patient["HEIGHT"]);
    query.addBindValue(patient["WEIGHT"]);
    query.addBindValue(patient["MOBILEPHONE"]);
    query.addBindValue(age);  // 使用计算出的年龄
    query.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    if (query.exec()) {
        qDebug() << "添加患者成功: ID=" << newId << ", 姓名=" << patient["NAME"].toString();
        addHistory("添加患者: " + patient["NAME"].toString() + " (ID: " + newId + ")");
        return true;
    }

    qDebug() << "添加患者失败:" << query.lastError().text();
    return false;
}

bool Database::updatePatient(const QString &id, const QMap<QString, QVariant> &patient)
{
    // 自动计算年龄
    QString dobStr = patient["DOB"].toString();
    int age = 0;
    if (!dobStr.isEmpty()) {
        QDate dob = QDate::fromString(dobStr, "yyyy-MM-dd");
        if (dob.isValid()) {
            QDate currentDate = QDate::currentDate();
            age = currentDate.year() - dob.year();

            // 如果今年还没过生日，年龄减1
            if (currentDate.month() < dob.month() ||
                (currentDate.month() == dob.month() && currentDate.day() < dob.day())) {
                age--;
            }
        }
    }

    QSqlQuery query;
    query.prepare("UPDATE Patient SET ID_CARD = ?, NAME = ?, SEX = ?, DOB = ?, HEIGHT = ?, WEIGHT = ?, MOBILEPHONE = ?, AGE = ? WHERE ID = ?");

    query.addBindValue(patient["ID_CARD"]);
    query.addBindValue(patient["NAME"]);
    query.addBindValue(patient["SEX"]);
    query.addBindValue(patient["DOB"]);
    query.addBindValue(patient["HEIGHT"]);
    query.addBindValue(patient["WEIGHT"]);
    query.addBindValue(patient["MOBILEPHONE"]);
    query.addBindValue(age);  // 使用计算出的年龄
    query.addBindValue(id);

    if (query.exec()) {
        qDebug() << "更新患者成功: ID=" << id << ", 姓名=" << patient["NAME"].toString();
        addHistory("更新患者信息: " + patient["NAME"].toString() + " (ID: " + id + ")");
        return true;
    }

    qDebug() << "更新患者失败:" << query.lastError().text();
    return false;
}

bool Database::deletePatient(const QString &id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM Patient WHERE ID = ?");
    query.addBindValue(id);

    if (query.exec()) {
        addHistory("删除患者ID: " + id);
        return true;
    }
    qDebug() << "删除患者失败:" << query.lastError().text();
    return false;
}

void Database::addHistory(const QString &event)
{
    if (currentUserId.isEmpty()) return;

    QSqlQuery query;
    query.prepare("INSERT INTO History (ID, USER_ID, EVENT, TIMESTAMP) VALUES (?, ?, ?, ?)");
    query.addBindValue(generateId());
    query.addBindValue(currentUserId);
    query.addBindValue(event);
    query.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    if (!query.exec()) {
        qDebug() << "添加历史记录失败:" << query.lastError().text();
    }
}

QList<QMap<QString, QString>> Database::getDoctors()
{
    QList<QMap<QString, QString>> doctors;

    QSqlQuery query("SELECT ID, EMPLOYEENO, NAME, DEPARTMENT_ID FROM Doctor ORDER BY ID");

    while (query.next()) {
        QMap<QString, QString> doctor;
        doctor["ID"] = query.value("ID").toString();
        doctor["EMPLOYEENO"] = query.value("EMPLOYEENO").toString();
        doctor["NAME"] = query.value("NAME").toString();
        doctor["DEPARTMENT_ID"] = query.value("DEPARTMENT_ID").toString();
        doctors.append(doctor);
    }

    qDebug() << "从数据库获取医生数量:" << doctors.size();
    return doctors;
}

QList<QMap<QString, QString>> Database::getDepartments()
{
    QList<QMap<QString, QString>> departments;

    QSqlQuery query("SELECT ID, NAME FROM Department ORDER BY ID");

    while (query.next()) {
        QMap<QString, QString> dept;
        dept["ID"] = query.value("ID").toString();
        dept["NAME"] = query.value("NAME").toString();
        departments.append(dept);
    }

    qDebug() << "从数据库获取科室数量:" << departments.size();
    return departments;
}

QString Database::generateId()
{
    return "ID_" + QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");
}
