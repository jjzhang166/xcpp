######################################################################
# Automatically generated by qmake (2.01a) ?? 10? 11 17:48:39 2014
######################################################################
include(../qtscript/win_noneUnicode.pri)
include(../qtscript/win_static.pri)
TEMPLATE = lib
CONFIG += staticlib debug_and_release thread

unix {
	APPNAME = xcpp
} else {
	APPNAME = libxcpp 
}

TARGET = $$APPNAME

BuildBrowserInformation = unicode

CONFIG(debug,debug|release) {
	DESTDIR = ../lib/Debug
	OBJECTS_DIR = ../temp/$$APPNAME/Debug
} else {
	DESTDIR = ../lib/Release
	OBJECTS_DIR = ../temp/$$APPNAME/Release
}

QT -= gui
QT -= core
DEPENDPATH += . ../src
INCLUDEPATH += . ../src

unix {
}

# Input
HEADERS += ../src/XApplication.h \
           ../src/ethCrc32.h \
           ../src/MD5c.h \
           stdafx.h \
           ../src/StringHash.h \
           ../src/XAfx.h \
		   ../src/XVersion.h \
           ../src/XEndian.h \
           ../src/XFileHelper.h \
           ../src/XLinux_P.h \
           ../src/XLog.h \
           ../src/XLogDefines.h \
           ../src/XMacro.h \
           ../src/XPipe.h \
           ../src/XPosix_P.h \
           ../src/XProcess.h \
           ../src/XSemaphore.h \
           ../src/XSettings.h \
           ../src/XSocket.h \
           ../src/XTcpSocket.h \
           ../src/XThread.h \
           ../src/XUdpSocket.h \
           ../src/XUtils.h \
		   ../src/XEString.h \
		   ../src/XFile.h \
		   ../src/XSTLMacro.h \
		   ../src/XSharedMemory.h
		   
		   
SOURCES += ../src/XApplication.cpp \
			../src/ethCrc32.cpp \
			../src/MD5c.cpp \
			../src/stdafx.cpp \
			../src/StringHash.cpp \
			../src/XLog.cpp \
			../src/XPipe.cpp \
			../src/XProcess.cpp \
			../src/XSemaphore.cpp \
			../src/XSettings.cpp \
			../src/XSocket.cpp \
			../src/XTcpSocket.cpp \
           ../src/XThread.cpp \
           ../src/XUdpSocket.cpp \
           ../src/XUtils.cpp \
		   ../src/XEString.cpp \
   		   ../src/XFile.cpp \
  		   ../src/XSharedMemory.cpp


VERSION = 0.6.0