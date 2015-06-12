TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -pthread

SOURCES += main.cpp \
    tcpserialsocket.cpp \
    logger.cpp \
    loggingobj.cpp

HEADERS += \
    tcpserialsocket.h \
    logger.h \
    loggingobj.h

QMAKE_CXXFLAGS += -std=c++11

TARGET = serial-client-logger
