QT=

TARGET = valueobj
TEMPLATE = lib

include ( src/value.pri )

QMAKE_CXXFLAGS += -D__STDC_FORMAT_MACROS
