#-------------------------------------------------
#
# Project created by QtCreator 2016-01-01T23:18:03
# Operating System Curriculum Design: Storage Manage
# Li Dongming(20133097),Xie Jia(20135019)
# Mail: Flyshit@cqu.edu.cn
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StorageSim
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    tlb.cpp \
    pagetable.cpp \
    disk.cpp \
    memory.cpp \
    processsim.cpp \
    storagesim.cpp

HEADERS  += mainwindow.h \
    tlb.h \
    pagetable.h \
    disk.h \
    memory.h \
    common.h \
    processsim.h \
    storagesim.h

FORMS    += mainwindow.ui
