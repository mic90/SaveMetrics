#-------------------------------------------------
#
# Project created by QtCreator 2016-10-19T18:39:56
#
#-------------------------------------------------

QT       -= core gui

TARGET = google-test
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    ../src/gtest.cc \
    ../src/gtest_main.cc \
    ../src/gtest-all.cc \
    ../src/gtest-death-test.cc \
    ../src/gtest-filepath.cc \
    ../src/gtest-port.cc \
    ../src/gtest-printers.cc \
    ../src/gtest-test-part.cc \
    ../src/gtest-typed-test.cc

HEADERS += \
    ../src/gtest-internal-inl.h \
    ../include/gtest/gtest.h \
    ../include/gtest/gtest_pred_impl.h \
    ../include/gtest/gtest_prod.h \
    ../include/gtest/gtest-death-test.h \
    ../include/gtest/gtest-message.h \
    ../include/gtest/gtest-param-test.h \
    ../include/gtest/gtest-param-test.h.pump \
    ../include/gtest/gtest-printers.h \
    ../include/gtest/gtest-spi.h \
    ../include/gtest/gtest-test-part.h \
    ../include/gtest/gtest-typed-test.h \
    ../include/gtest/internal/gtest-death-test-internal.h \
    ../include/gtest/internal/gtest-filepath.h \
    ../include/gtest/internal/gtest-internal.h \
    ../include/gtest/internal/gtest-linked_ptr.h \
    ../include/gtest/internal/gtest-param-util.h \
    ../include/gtest/internal/gtest-param-util-generated.h \
    ../include/gtest/internal/gtest-port.h \
    ../include/gtest/internal/gtest-port-arch.h \
    ../include/gtest/internal/gtest-string.h \
    ../include/gtest/internal/gtest-tuple.h \
    ../include/gtest/internal/gtest-tuple.h.pump \
    ../include/gtest/internal/gtest-type-util.h \
    ../include/gtest/internal/custom/gtest.h \
    ../include/gtest/internal/custom/gtest-port.h \
    ../include/gtest/internal/custom/gtest-printers.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
win32 {
    DESTDIR = $$PWD/../../lib
}

INCLUDEPATH += ../include/ \
               ../

DISTFILES += \
    ../include/gtest/internal/gtest-param-util-generated.h.pump \
    ../include/gtest/internal/gtest-type-util.h.pump
