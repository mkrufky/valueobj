QT=

TARGET = valueobj
TEMPLATE = lib

include ( src/value.pri )

SOURCES += $$VALOBJ_SOURCES

QMAKE_CXXFLAGS += -D__STDC_FORMAT_MACROS
