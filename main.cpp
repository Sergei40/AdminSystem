#include <QApplication>
#include "server.h"

#include <QVBoxLayout>
#include <QWebEngineView>
#include <QLineEdit>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    Server server;
    server.start();

//    QWidget window;
//    QVBoxLayout layout(&window);

//    QWebEngineView view;
//    QLineEdit urlLineEdit;

//    QObject::connect(&view, &QWebEngineView::urlChanged, [&urlLineEdit](const QUrl& url) {
//        urlLineEdit.setText(url.toString());
//    });

//    view.load(QUrl("http://localhost:1234"));

//    layout.addWidget(&urlLineEdit);
//    layout.addWidget(&view);

//    window.setMinimumSize(1000, 700);
//    window.show();

    return app.exec();
}
