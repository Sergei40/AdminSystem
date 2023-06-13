#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QHttpServer>

#include "databasemanager.h"

class Server : public QObject {
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);

    void start();
    void showWebView(QHttpServerResponse &response);
private:
    QHttpServer server;
    DatabaseManager dataBaseManager;

    void handleRequest(const QHttpServerRequest &request, QHttpServerResponse &response);
    void sendHtmlFile(QHttpServerResponse &response);
    void getUsers(QHttpServerResponse &response, int page);
    void getUser(QHttpServerResponse &response, int userId);
    void addUser(QHttpServerResponse &response, const QByteArray &requestData);
    void updateUser(QHttpServerResponse &response, int userId, const QByteArray &requestData);
    void removeUser(QHttpServerResponse &response, int userId);
    void sendResponse(QHttpServerResponse &response, QHttpServerResponse::StatusCode statusCode,
                      const QByteArray &contentType, const QByteArray &contentData);
    int getPageFromHeaders(const QHttpServerRequest &request);
};

#endif // SERVER_H
