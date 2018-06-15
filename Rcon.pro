#-------------------------------------------------
#
# Project created by QtCreator 2018-06-09T14:33:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Rcon
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h

FORMS += \
        mainwindow.ui

unix:!macx: LIBS += -L$$PWD/../../../../../../../../usr/local/lib/ -lraw

INCLUDEPATH += $$PWD/../../../../../../../../usr/local/include
DEPENDPATH += $$PWD/../../../../../../../../usr/local/include

unix:!macx: PRE_TARGETDEPS += $$PWD/../../../../../../../../usr/local/lib/libraw.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../../usr/local/lib/release/ -lraw
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../../usr/local/lib/debug/ -lraw
else:unix: LIBS += -L$$PWD/../../../../../../../../usr/local/lib/ -lraw

INCLUDEPATH += $$PWD/../../../../../../../../usr/local/include/libraw
DEPENDPATH += $$PWD/../../../../../../../../usr/local/include/libraw

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../../../usr/local/lib/release/libraw.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../../../usr/local/lib/debug/libraw.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../../../usr/local/lib/release/raw.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../../../usr/local/lib/debug/raw.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../../../../../../../usr/local/lib/libraw.a
