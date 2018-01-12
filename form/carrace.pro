#-------------------------------------------------
#
# Project created by QtCreator 2016-11-15T15:03:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = carrace
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    mythread.cpp \
    enthread.cpp

HEADERS  += widget.h \
    mythread.h \
    enthread.h

FORMS    += widget.ui

win32:CONFIG(release, debug|release): LIBS += -LD:/rpclib-master/X64Build/release/ -lrpc

INCLUDEPATH += D:/rpclib-master/include
DEPENDPATH += D:/rpclib-master/include


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../opencv/build/x64/vc14/lib/ -lopencv_world320
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../opencv/build/x64/vc14/lib/ -lopencv_world320d

INCLUDEPATH += D:\opencv\build\include
               D:\opencv\build\include\opencv
               D:\opencv\build\include\opencv2


DEPENDPATH += D:\opencv\build\include
               D:\opencv\build\include\opencv
               D:\opencv\build\include\opencv2

win32:CONFIG(release, debug|release): LIBS += -LC:/Qt/Qt5.7.1/5.7/msvc2015_64/lib/ -lQt5Widgets
else:win32:CONFIG(debug, debug|release): LIBS += -LC:/Qt/Qt5.7.1/5.7/msvc2015_64/lib/ -lQt5Widgetsd

INCLUDEPATH += C:/Qt/Qt5.7.1/5.7/msvc2015_64/include
DEPENDPATH += C:/Qt/Qt5.7.1/5.7/msvc2015_64/include
