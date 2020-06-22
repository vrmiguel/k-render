TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        src/kio.cpp \
        src/krender.cpp \
        src/ktypes.cpp \
        src/main.cpp

HEADERS += \
    includes/kio.h \
    includes/krender.h \
    includes/ktypes.h \
    includes/kvec.h
