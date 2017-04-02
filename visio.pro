#-------------------------------------------------
#
# Project created by QtCreator 2015-10-04T15:36:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = visio
TEMPLATE = app

#LIBS += -L/usr/lib/x86_64-linux-gnu
LIBS += -L/usr/lib/x86_64-linux-gnu/hdf5/serial/lib -lhdf5 -lhdf5_cpp -lhdf5_hl

INCLUDEPATH += /usr/lib/x86_64-linux-gnu/hdf5/serial/include

SOURCES += main.cpp\
        color_selector.cpp \
    render_area.cpp \
    visualizator.cpp \
    camera.cpp \
    frame.cpp \
    render_universe.cpp \
    movie.cpp \
    text.cpp \
    rawhdata.cpp

HEADERS  += color_selector.h \
    render_area.h \
    visualizator.h \
    camera.h \
    frame.h \
    render_universe.h \
    movie.h \
    text.h \
    rawhdata.h

FORMS    += \
    color_selector.ui \
    visualizator.ui

