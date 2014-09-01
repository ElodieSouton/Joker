#-------------------------------------------------
#
# Project created by QtCreator 2014-01-27T17:15:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LTCTool
TEMPLATE = app

VERSION = 1.0.0

DEFINES += APP_NAME=\\\"$$TARGET\\\"

CONFIG += ltc

INCLUDEPATH += ../../libs

include(../../libs/PhTools/PhTools.pri);
include(../../libs/PhAudio/PhAudio.pri);
include(../../libs/PhSync/PhSync.pri);
include(../../libs/PhLtc/PhLtc.pri);
include(../../libs/PhCommonUI/PhCommonUI.pri);

SOURCES += main.cpp\
		LTCToolWindow.cpp \
	PreferencesDialog.cpp

HEADERS  += LTCToolWindow.h \
	PreferencesDialog.h \
	LTCToolSettings.h

FORMS    += LTCToolWindow.ui \
	PreferencesDialog.ui

mac {
	# For the plist version
	QMAKE_INFO_PLIST +=  $${JOKER_ROOT}/data/test.plist
}

PH_DEPLOY_LOCATION = $$(JOKER_RELEASE_PATH)
include(../../common/deploy.pri)
