QT=

TARGET = valueobj
TEMPLATE = lib
CONFIG += staticlib
DESTDIR = ..

include ( value.pri )

SOURCES += $$VALOBJ_SOURCES

QMAKE_CXXFLAGS += -D__STDC_FORMAT_MACROS
