AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = libiz.la

libiz_la_SOURCES = decode.cpp encode.cpp portableimage.cpp table.cpp

libiz_la_LIBADD = @LTLIBOBJS@
libiz_la_LDFLAGS = -release @VERSION@

include_HEADERS = bitcoder.h intmacros.h iz_p.h libiz.h pixel.h portableimage.h
EXTRA_DIST = bitcoder.h intmacros.h iz_p.h libiz.h pixel.h portableimage.h
noinst_HEADERS = bitcoder.h intmacros.h iz_p.h libiz.h pixel.h portableimage.h

iz_test_SOURCES = main.cpp
iz_test_LDADD = $(lib_LTLIBRARIES)
bin_PROGRAMS = iz_test
