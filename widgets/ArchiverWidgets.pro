#-------------------------------------------------
#
# Project Archiver
#
#-------------------------------------------------

QT += core
QT += gui
QT += widgets

# c++17
#
CONFIG += c++17

# Title
#
TARGET = ArchiverWidgets
TEMPLATE = app

#Application icon
#
win32:RC_ICONS += ../icons/Archiver.ico

# DESTDIR
#
win32 {
    CONFIG(debug, debug|release): DESTDIR = ../../bin/debug
        CONFIG(release, debug|release): DESTDIR = ../../bin/release
}
unix {
    CONFIG(debug, debug|release): DESTDIR = ../../bin_unix/debug
        CONFIG(release, debug|release): DESTDIR = ../../bin_unix/release
}

# FILES
#
SOURCES += \
    ../ArchFile.cpp \
    ../ArchWorker.cpp \
    ../BarchArchFile.cpp \
    ../BmpArchFile.cpp \
    ../FilesModel.cpp \
    FilesView.cpp \
    ../WorkersBase.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    ../ArchFile.h \
    ../ArchWorker.h \
    ../BarchArchFile.h \
    ../BmpArchFile.h \
    ../FilesModel.h \
    FilesView.h \
    MainWindow.h \
    ../WorkersBase.h

RESOURCES += \
    Resources.qrc

#TRANSLATIONS = languages/Archiver_ua.ts

CONFIG += ../precompile_header
PRECOMPILED_HEADER = Stable.h
