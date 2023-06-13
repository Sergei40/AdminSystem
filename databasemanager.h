#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVector>
#include "user.h"

class DatabaseManager {
public:
    DatabaseManager();

    bool addUser(const User& user);
    bool updateUser(const User& user);
    bool deleteUser(int id);
    QVector<User> getUsers();
    User getUser(int userId);

private:
    QSqlDatabase database;

    bool openDatabase();
    void closeDatabase();
    bool createTable();
};

#endif // DATABASEMANAGER_H
