#-------------------------------------------------
#
# Project created by QtCreator 2014-01-27T17:15:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AudioTest
TEMPLATE = app

VERSION = 1.0.0

DEFINES += APP_NAME=\\\"$$TARGET\\\"

INCLUDEPATH += ../../libs

include(../../libs/PhTools/PhTools.pri);
include(../../libs/PhAudio/PhAudio.pri);

SOURCES += main.cpp\
        AudioTestWindow.cpp \
    PreferencesDialog.cpp \
    AudioTestWriter.cpp

HEADERS  += AudioTestWindow.h \
    PreferencesDialog.h \
    AudioTestWriter.h

FORMS    += AudioTestWindow.ui \
    PreferencesDialog.ui

PH_DEPLOY_LOCATION = $$(TESTS_RELEASE_PATH)
include(../../common/deploy.pri)
