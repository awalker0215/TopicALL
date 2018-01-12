QT += core
QT += sql
QT -= gui
CONFIG += c++11

TARGET = testServerDB
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp
INCLUDEPATH += C:/ProgramFiles/MySQL/MySQLServer5.7/include
QMAKE_LIBDIR += C:/ProgramFiles/MySQL/MySQLServer5.7/lib
LIBS += -lmysqlcppconn

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32:CONFIG(release, debug|release): LIBS += -LD:/opencv/opencv/build/x64/vc14/lib/ -lopencv_world310
else:win32:CONFIG(debug, debug|release): LIBS += -LD:/opencv/opencv/build/x64/vc14/lib/ -lopencv_world310d
else:unix: LIBS += -LD:/opencv/opencv/build/x64/vc14/lib/ -lopencv_world310

INCLUDEPATH += D:\opencv\opencv\build\include
               D:\opencv\opencv\build\include\opencv
               D:\opencv\opencv\build\include\opencv2


DEPENDPATH += D:\opencv\opencv\build\include
               D:\opencv\opencv\build\include\opencv
               D:\opencv\opencv\build\include\opencv2

HEADERS += \
    transmitter.h \
    transmitter.hpp \
    transmitter.h


win32:CONFIG(release, debug|release): LIBS += -LD:/rpclib-master/X64Build/release/ -lrpc

INCLUDEPATH += D:/rpclib-master/include
DEPENDPATH += D:/rpclib-master/include


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/'../../../../../../Program Files (x86)/MySQL/Connector ODBC 5.3/' -lmyodbc5a
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/'../../../../../../Program Files (x86)/MySQL/Connector ODBC 5.3/' -lmyodbc5ad

INCLUDEPATH += $$PWD/'../../../../../../Program Files (x86)/MySQL/Connector ODBC 5.3'
DEPENDPATH += $$PWD/'../../../../../../Program Files (x86)/MySQL/Connector ODBC 5.3'

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/'../../../../../../Program Files (x86)/MySQL/MySQL Connector C++ 1.1.9/lib/opt/' -lmysqlcppconn
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/'../../../../../../Program Files (x86)/MySQL/MySQL Connector C++ 1.1.9/lib/opt/' -lmysqlcppconnd

INCLUDEPATH += $$PWD/'../../../../../../Program Files (x86)/MySQL/MySQL Connector C++ 1.1.9/include'
DEPENDPATH += $$PWD/'../../../../../../Program Files (x86)/MySQL/MySQL Connector C++ 1.1.9/include'


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../Downloads/mysql-5.7.20-win32/lib/ -llibmysql
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../Downloads/mysql-5.7.20-win32/lib/ -llibmysqld

INCLUDEPATH += $$PWD/../../../../Downloads/mysql-5.7.20-win32/include
DEPENDPATH += $$PWD/../../../../Downloads/mysql-5.7.20-win32/include


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/'../../../../../../Program Files (x86)/MySQL/MySQL Connector C 6.1/lib/' -llibmysql
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/'../../../../../../Program Files (x86)/MySQL/MySQL Connector C 6.1/lib/' -llibmysqld

INCLUDEPATH += $$PWD/'../../../../../../Program Files (x86)/MySQL/MySQL Connector C 6.1/include'
DEPENDPATH += $$PWD/'../../../../../../Program Files (x86)/MySQL/MySQL Connector C 6.1/include'



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../Downloads/mysql-connector-c-6.1.11-win32/lib/ -llibmysql
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../Downloads/mysql-connector-c-6.1.11-win32/lib/ -llibmysqld

INCLUDEPATH += $$PWD/../../../../Downloads/mysql-connector-c-6.1.11-win32/include
DEPENDPATH += $$PWD/../../../../Downloads/mysql-connector-c-6.1.11-win32/include



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/'../../../../../../Program Files/MySQL/MySQL Server 5.7/lib/' -llibmysql
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/'../../../../../../Program Files/MySQL/MySQL Server 5.7/lib/' -llibmysqld

INCLUDEPATH += $$PWD/'../../../../../../Program Files/MySQL/MySQL Server 5.7/include'
DEPENDPATH += $$PWD/'../../../../../../Program Files/MySQL/MySQL Server 5.7/include'
