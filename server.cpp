#include "server.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <iostream>

Server::Server(QObject *parent) : QObject(parent) {
    server.afterRequest([this](const QHttpServerRequest &request, QHttpServerResponse &&response) {
        handleRequest(request, response);
        QHttpServerResponse res(std::move(response));
        return res;
    });
}

void Server::start() {
    server.listen(QHostAddress::Any, 1234);
    qDebug() << "Server started";
}

void Server::handleRequest(const QHttpServerRequest &request, QHttpServerResponse &response) {
    QHttpServerRequest::Method method = request.method();
    QString path = request.url().path();

    if (method == QHttpServerRequest::Method::Get && path == "/") {
        sendHtmlFile(response);
    } else if (method == QHttpServerRequest::Method::Get && path == "/users") {
        int page = getPageFromHeaders(request);
        getUsers(response, page);
    } else if (method == QHttpServerRequest::Method::Post && path == "/users") {
        addUser(response, request.body());
    } else if (method == QHttpServerRequest::Method::Put && path.startsWith("/users/")) {
        QString userIdString = path.split("/").last();
        bool ok;
        int userId = userIdString.toInt(&ok);
        if (ok) {
            updateUser(response, userId, request.body());
        } else {
            sendResponse(response, QHttpServerResponse::StatusCode::BadRequest, "text/plain", "Invalid user ID");
        }
    } else if (method == QHttpServerRequest::Method::Delete && path.startsWith("/users/")) {
        QString userIdString = path.split("/").last();
        bool ok;
        int userId = userIdString.toInt(&ok);
        if (ok) {
            removeUser(response, userId);
        } else {
            sendResponse(response, QHttpServerResponse::StatusCode::BadRequest, "text/plain", "Invalid user ID");
        }
    } else if (method == QHttpServerRequest::Method::Get && path.startsWith("/users/")) {
        QString userIdString = path.split("/").last();
        bool ok;
        int userId = userIdString.toInt(&ok);
        if (ok) {
            getUser(response, userId);
        } else {
            sendResponse(response, QHttpServerResponse::StatusCode::BadRequest, "text/plain", "Invalid user ID");
        }
    } else {
        sendResponse(response, QHttpServerResponse::StatusCode::NotFound, "text/plain", "404 Not Found");
    }
}

void Server::sendHtmlFile(QHttpServerResponse &response) {
    QFile file(":/index.html");
    if (!file.open(QIODevice::ReadOnly)) {
        sendResponse(response, QHttpServerResponse::StatusCode::InternalServerError, "text/plain", "Failed to open HTML file");
        return;
    }

    QByteArray fileData = file.readAll();
    sendResponse(response, QHttpServerResponse::StatusCode::Ok, "text/html", fileData);
}

void Server::getUsers(QHttpServerResponse &response, int page) {
    const int entriesPerPage = 5;
    int totalUsers = dataBaseManager.getUsers().size();
    int totalPages = totalUsers / entriesPerPage + (totalUsers % entriesPerPage != 0);

    if (page < 1 || page > totalPages) {
        sendResponse(response, QHttpServerResponse::StatusCode::BadRequest, "text/plain", "Invalid page number");
        return;
    }

    int startIndex = (page - 1) * entriesPerPage;
    int endIndex = std::min(startIndex + entriesPerPage, totalUsers);

    QJsonArray usersArray;
    for (int i = startIndex; i < endIndex; ++i) {
        User user = dataBaseManager.getUsers().at(i);
        QJsonObject userObject;
        userObject["id"] = user.getId();
        userObject["fullName"] = user.getFullName();
        userObject["dob"] = user.getBirthDate().toString("dd.MM.yyyy");
        userObject["role"] = user.getRole();
        userObject["loginTime"] = user.getLoginTime().toString("dd.MM.yyyy HH:mm:ss");
        usersArray.append(userObject);
    }

    QJsonObject responseObj;
    responseObj["users"] = usersArray;
    responseObj["currentPage"] = page;
    responseObj["totalPages"] = totalPages;

    QJsonDocument jsonDoc(responseObj);
    sendResponse(response, QHttpServerResponse::StatusCode::Ok, "application/json", jsonDoc.toJson());
}

void Server::getUser(QHttpServerResponse &response, int userId) {
    User user = dataBaseManager.getUser(userId);

    if (user.isValid()) {
        QJsonObject userObject;
        userObject["id"] = user.getId();
        userObject["fullName"] = user.getFullName();
        userObject["dob"] = user.getBirthDate().toString("dd.MM.yyyy");
        userObject["role"] = user.getRole();
        userObject["loginTime"] = user.getLoginTime().toString("dd.MM.yyyy HH:mm:ss");

        QJsonDocument jsonDoc(userObject);
        sendResponse(response, QHttpServerResponse::StatusCode::Ok, "application/json", jsonDoc.toJson());
    } else {
        sendResponse(response, QHttpServerResponse::StatusCode::NotFound, "text/plain", "User not found");
    }
}

void Server::addUser(QHttpServerResponse &response, const QByteArray &requestData) {
    QJsonDocument jsonDoc = QJsonDocument::fromJson(requestData);
    QJsonObject userObject = jsonDoc.object();
    try {
        User user(userObject);
        dataBaseManager.addUser(user);
        sendResponse(response, QHttpServerResponse::StatusCode::Created, "text/plain", "User added successfully");
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        QByteArray errorInfo = QString("Failed to add user: " + QString(ex.what())).toUtf8();
        sendResponse(response, QHttpServerResponse::StatusCode::BadRequest, "text/plain", errorInfo);
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        sendResponse(response, QHttpServerResponse::StatusCode::InternalServerError, "text/plain", "Failed to add user. An unknown error occurred.");
    }
}

void Server::updateUser(QHttpServerResponse &response, int userId, const QByteArray &requestData) {
    QJsonDocument jsonDoc = QJsonDocument::fromJson(requestData);
    QJsonObject userObject = jsonDoc.object();
    try {
        User user(userObject);
        user.setId(userId);
        dataBaseManager.updateUser(user);
        sendResponse(response, QHttpServerResponse::StatusCode::Ok, "text/plain", "User updated successfully");
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        QByteArray errorInfo = QString("Failed to update user: " + QString(ex.what())).toUtf8();
        sendResponse(response, QHttpServerResponse::StatusCode::BadRequest, "text/plain", errorInfo);
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        sendResponse(response, QHttpServerResponse::StatusCode::InternalServerError, "text/plain", "Failed to update user. An unknown error occurred.");
    }
}

void Server::removeUser(QHttpServerResponse &response, int userId) {
    bool removed = dataBaseManager.deleteUser(userId);

    if (removed) {
        sendResponse(response, QHttpServerResponse::StatusCode::Ok, "text/plain", "User removed successfully");
    } else {
        sendResponse(response, QHttpServerResponse::StatusCode::NotFound, "text/plain", "User not found");
    }
}

void Server::sendResponse(QHttpServerResponse &response, QHttpServerResponse::StatusCode statusCode,
                  const QByteArray &contentType, const QByteArray &contentData) {

    response = QHttpServerResponse(contentType, contentData, statusCode);
}

int Server::getPageFromHeaders(const QHttpServerRequest &request) {
    QUrl url = request.url();
    QUrlQuery urlQuery(url.query());
    QString pageValue = urlQuery.queryItemValue("page");
    bool conversionOk = false;
    int page = pageValue.toInt(&conversionOk);
    if (!conversionOk) {
        page = 1; // Значение по умолчанию, если преобразование не удалось
    }
    return page;
}

