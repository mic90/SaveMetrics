#-------------------------------------------------
#
# Project created by QtCreator 2017-05-21T13:24:11
#
#-------------------------------------------------

QT       += core gui widgets printsupport
CONFIG   += c++14
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtLua
TEMPLATE = app

DESTDIR = $$PWD/../bin
DEFINES += QS_LOG_LINE_NUMBERS

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

LIBS += -L$$PWD/../external/luajit-2.0.4/src -lluajit \
        -L$$PWD/../external/qslog/lib/ -lQsLog \
        -L$$PWD/../external/QScintilla_gpl-2.10\lib -lqscintilla2_qt5

INCLUDEPATH += $$PWD/../external/luajit-2.0.4/src \
               $$PWD/../external/sol2/single/sol/ \
               $$PWD/../external/qslog/ \
               $$PWD/../external/QScintilla_gpl-2.10\Qt4Qt5 \
               $$PWD/../external/qcustomplot/

DEPENDPATH += $$PWD/../external/luajit-2.0.4/src \
              $$PWD/../external/sol2/single/sol/ \
              $$PWD/../external/qslog/ \
              $$PWD/../external/QScintilla_gpl-2.10\Qt4Qt5 \
              $$PWD/../external/qcustomplot/

SOURCES += main.cpp\
        mainwindow.cpp \
    script/luacheck.cpp \
    script/lualogger.cpp \
    script/luarocks.cpp \
    script/scriptenginelua.cpp \
    script/scriptenginerunner.cpp \
    script/scriptfile.cpp \
    project/projectfile.cpp \
    project/project.cpp \
    ui/widget_luaeditor.cpp \
    ui/window_scriptfile.cpp \
    ../external/qcustomplot/qcustomplot.cpp \
    ui/window_charts.cpp \
    ui/filesiconprovider.cpp

HEADERS  += mainwindow.h \
    script/luacheck.h \
    script/lualogger.h \
    script/luarocks.h \
    script/scriptenginelua.h \
    script/scriptenginerunner.h \
    script/scriptfile.h \
    project/projectfile.h \
    project/project.h \
    ui/widget_luaeditor.h \
    ui/window_scriptfile.h \
    ../external/qcustomplot/qcustomplot.h \
    ui/window_charts.h \
    ui/filesiconprovider.h

FORMS    += mainwindow.ui \
    window_scriptfile.ui \
    window_charts.ui

RESOURCES += \
    icons.qrc
