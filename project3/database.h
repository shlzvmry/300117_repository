//database.h
#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>
#include <QMap>
#include <QVariant>

class Database : public QObject
{
    Q_OBJECT
public:
    static Database& instance();
    bool init();
    QSqlDatabase getDatabase() { return db; }

    // 用户操作
    bool login(const QString &username, const QString &password);
    bool registerUser(const QString &fullname, const QString &username, const QString &password);

    // 患者操作
    QList<QMap<QString, QVariant>> getPatients(const QString &filter = "");
    bool addPatient(const QMap<QString, QVariant> &patient);
    bool updatePatient(const QString &id, const QMap<QString, QVariant> &patient);
    bool deletePatient(const QString &id);

    // 历史记录操作
    void addHistory(const QString &event);

    // 获取医生和科室信息（从数据库）
    QList<QMap<QString, QString>> getDoctors();
    QList<QMap<QString, QString>> getDepartments();

private:
    explicit Database(QObject *parent = nullptr);
    QSqlDatabase db;
    QString currentUserId;

    QString generateId();
};

#endif // DATABASE_H
