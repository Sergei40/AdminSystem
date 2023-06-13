QT += core network widgets sql httpserver webenginewidgets
CONFIG += c++17

TARGET = AdminSystem
TEMPLATE = app

SOURCES += main.cpp \
    databasemanager.cpp \
    server.cpp \
    user.cpp \

HEADERS += \
    databasemanager.h \
    server.h \
    user.h \

# Дополнительные настройки
LIBS += -LD:/SQLite

# Добавляем веб-ресурсы (HTML, CSS и другие файлы)
RESOURCES += \
    res.qrc

DISTFILES += \
    index.html
