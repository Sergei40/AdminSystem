#include "user.h"
#include <QRegularExpression>
#include <stdexcept>

class UserValidator {
public:
    static bool isFullNameValid(const QString& fullName) {
        return !fullName.isEmpty();
    }

    static bool isBirthDateValid(const QString& dob) {
        QRegularExpression dateRegex("\\d{2}\\.\\d{2}\\.\\d{4}");
        return dateRegex.match(dob).hasMatch();
    }

    static bool isRoleValid(const QString& role) {
        return role == "Administrator" || role == "Guest" || role == "Developer";
    }

    static bool isLoginTimeValid(const QString& loginTime) {
        QRegularExpression dateTimeRegex("\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}");
        return dateTimeRegex.match(loginTime).hasMatch();
    }
};

User::User(const int id, const QString& fullName, const QDate& dob, const QString& role, const QDateTime& loginTime)
    : id_(id), fullName_(fullName), birthDate_(dob), role_(role), loginTime_(loginTime) {}


User::User(const QJsonObject& jsonObject) {
    if (!jsonObject.contains("fullName") || !jsonObject.contains("dob") ||
        !jsonObject.contains("role")) {
        throw std::invalid_argument("Invalid JSON object structure");
    }

    QString fullName = jsonObject["fullName"].toString();
    QString dob = jsonObject["dob"].toString();
    QString roleString = jsonObject["role"].toString();

    if (!UserValidator::isFullNameValid(fullName)) {
        throw std::invalid_argument("Invalid full name");
    }

    if (!UserValidator::isBirthDateValid(dob)) {
        throw std::invalid_argument("Invalid birth date");
    }

    if (!UserValidator::isRoleValid(roleString)) {
        throw std::invalid_argument("Invalid role");
    }


    id_ = -1;
    fullName_ = fullName;
    birthDate_ = QDate::fromString(dob, "dd.MM.yyyy");
    role_ = roleString;
    loginTime_ = QDateTime::currentDateTime();
}

User::User()
{
    isBad_ = true;
}

QString User::getFullName() const {
    return fullName_;
}

QDate User::getBirthDate() const {
    return birthDate_;
}

QString User::getRole() const {
    return role_;
}

QDateTime User::getLoginTime() const {
    return loginTime_;
}

int User::getId() const
{
    return id_;
}

void User::setId(int id)
{
    id_ = id;
}

bool User::isValid()
{
    return !isBad_;
}
