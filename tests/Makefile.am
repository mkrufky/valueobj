AM_CXXFLAGS = -I../src -I../igloo -w

check_PROGRAMS = igloo-object igloo-array

igloo_array_SOURCES = igloo-array.cpp igloo-listener.cpp
igloo_array_LDADD = -L../src -lvalueobj

igloo_object_SOURCES = igloo-object.cpp igloo-listener.cpp
igloo_object_LDADD = -L../src -lvalueobj

TESTS = $(check_PROGRAMS)
