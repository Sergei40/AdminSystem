#include "databasemanager.h"

#include <iostream>

DatabaseManager::DatabaseManager() {
    if (!openDatabase()) {
        std::cerr << "Failed to open the database." << std::endl;
    }

    if (!createTable()) {
        std::cerr << "Failed to create the table." << std::endl;
    }
}


bool DatabaseManager::openDatabase() {
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("users.db");

    if (!database.open()) {
        return false;
    }

    return true;
}

void DatabaseManager::closeDatabase() {
    database.close();
}

bool DatabaseManager::createTable() {
    QSqlQuery query(database);

    QString createQuery = "CREATE TABLE IF NOT EXISTS users ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                          "fullName TEXT,"
                          "dob TEXT,"
                          "role TEXT,"
                          "loginTime TEXT"
                          ")";

    if (!query.exec(createQuery)) {
        return false;
    }

    return true;
}

bool DatabaseManager::addUser(const User& user) {
    QSqlQuery query(database);

    query.prepare("INSERT INTO users (fullName, dob, role, loginTime) "
                  "VALUES (:fullName, :dob, :role, :loginTime)");

    query.bindValue(":fullName", user.getFullName());
    query.bindValue(":dob", user.getBirthDate().toString("dd.MM.yyyy"));
    query.bindValue(":role", user.getRole());
    query.bindValue(":loginTime", user.getLoginTime().toString("dd.MM.yyyy HH:mm:ss"));

    if (!query.exec()) {
        return false;
    }

    return true;
}

bool DatabaseManager::updateUser(const User& user) {
    QSqlQuery query(database);

    query.prepare("UPDATE users SET fullName = :fullName, dob = :dob, "
                  "role = :role, loginTime = :loginTime WHERE id = :id");

    query.bindValue(":fullName", user.getFullName());
    query.bindValue(":dob", user.getBirthDate().toString("dd.MM.yyyy"));
    query.bindValue(":role", user.getRole());
    query.bindValue(":loginTime", user.getLoginTime().toString("dd.MM.yyyy HH:mm:ss"));
    query.bindValue(":id", user.getId());

    if (!query.exec()) {
        return false;
    }

    return true;
}

bool DatabaseManager::deleteUser(int id) {
    QSqlQuery query(database);

    query.prepare("DELETE FROM users WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        return false;
    }

    return true;
}

QVector<User> DatabaseManager::getUsers() {
    QVector<User> users;

    QSqlQuery query(database);

    if (!query.exec("SELECT * FROM users")) {
        return users;
    }

    while (query.next()) {
        int id = query.value("id").toInt();
        QString fullName = query.value("fullName").toString();
        QDate dob = QDate::fromString(query.value("dob").toString(), "dd.MM.yyyy");
        QString role = query.value("role").toString();
        QDateTime loginTime = QDateTime::fromString(query.value("loginTime").toString(), "dd.MM.yyyy HH:mm:ss");

        User user(id, fullName, dob, role, loginTime);

        users.append(user);
    }

    return users;
}

User DatabaseManager::getUser(int userId) {
    QSqlQuery query(database);

    query.prepare("SELECT * FROM users WHERE id = :id");
    query.bindValue(":id", userId);

    if (!query.exec() || !query.next()) {
        return User();
    }

    int id = query.value("id").toInt();
    QString fullName = query.value("fullName").toString();
    QDate dob = QDate::fromString(query.value("dob").toString(), "dd.MM.yyyy");
    QString role = query.value("role").toString();
    QDateTime loginTime = QDateTime::fromString(query.value("loginTime").toString(), "dd.MM.yyyy HH:mm:ss");

    User user(id, fullName, dob, role, loginTime);

    return user;
}
