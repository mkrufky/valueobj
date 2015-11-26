QT=

TARGET = tests

TEMPLATE = app
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += testcase

INCLUDEPATH += $$PWD/../include/valobj $$PWD/../igloo

LIBS += $$PWD/../libvalueobj.a

OTHER_FILES += $$PWD/Makefile.am

HEADERS = igloo-listener.h

SOURCES += \
	igloo-array.cpp \
	igloo-object.cpp \
	igloo-listener.cpp
