QT=

TARGET = valueobj
TEMPLATE = lib
DESTDIR = .

include ( src/value.pri )

SOURCES += $$VALOBJ_SOURCES

QMAKE_CXXFLAGS += -D__STDC_FORMAT_MACROS

OTHER_FILES += AUTHORS \
               COPYING \
               INSTALL \
               NEWS \
               README \
               ChangeLog \
               configure.ac \
               Makefile.am \
               appveyor.yml \
               circle.yml \
               .travis.yml \
               version.m4
