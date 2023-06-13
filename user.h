#ifndef USER_H
#define USER_H

#include <QString>
#include <QDate>
#include <QDateTime>
#include <QJsonObject>

enum class UserRole {
    Administrator,
    Guest,
    Developer
};

class User {
public:
    User(const int id, const QString& fullName, const QDate& dob, const QString& role, const QDateTime& loginTime);
    User(const QJsonObject& jsonObject);
    User();
    QString getFullName() const;
    QDate getBirthDate() const;
    QString getRole() const;
    QDateTime getLoginTime() const;
    int getId() const;
    void setId(int id);
    bool isValid();

private:
    int id_;
    QString fullName_;
    QDate birthDate_;
    QString role_;
    QDateTime loginTime_;
    bool isBad_ = false;
};

#endif // USER_H
