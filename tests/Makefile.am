AM_CXXFLAGS = -I../include/valobj -I../igloo -w

if COVERALLS
AM_CXXFLAGS += -g -O0
endif COVERALLS

check_PROGRAMS = igloo-object igloo-array igloo-handle

igloo_array_SOURCES = igloo-array.cpp igloo-listener.cpp
igloo_array_LDADD = -L../src -lvalueobj

igloo_object_SOURCES = igloo-object.cpp igloo-listener.cpp
igloo_object_LDADD = -L../src -lvalueobj

igloo_handle_SOURCES = igloo-handle.cpp igloo-listener.cpp
igloo_handle_LDADD = -L../src -lvalueobj

TESTS = $(check_PROGRAMS)
