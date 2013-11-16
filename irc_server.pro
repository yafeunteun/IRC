SOURCES += \
    main.cpp \
    server.cpp \
    client.cpp \
    channel.cpp \
    command.cpp \
    frame.cpp

QT = network widgets
QT += gui
QT += core

HEADERS += \
    server.h \
    client.h \
    channel.h \
    command.h \
    frame.h
