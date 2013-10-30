SOURCES += \
    main.cpp \
    server.cpp \
    client.cpp \
    channel.cpp \
    bdPlatformLog.cpp \
    command.cpp \
    frame.cpp

QT = network widgets
QT += gui
QT += core

HEADERS += \
    server.h \
    client.h \
    channel.h \
    bdPlatformLog.h \
    command.h \
    frame.h
