AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = libiz.la

libiz_la_SOURCES = libiz.cpp table.cpp

libiz_la_LIBADD = @LTLIBOBJS@
libiz_la_LDFLAGS = -release @VERSION@

include_HEADERS = bitcoder.h encoder.h decoder.h file.h image.h intmacros.h iz_p.h libiz.h pixel.h portableimage.h predict.h
EXTRA_DIST = bitcoder.h encoder.h decoder.h file.h image.h intmacros.h iz_p.h libiz.h pixel.h portableimage.h predict.h
noinst_HEADERS = bitcoder.h encoder.h decoder.h file.h image.h intmacros.h iz_p.h libiz.h pixel.h portableimage.h predict.h

iz_test_SOURCES = file.cpp main.cpp portableimage.cpp
iz_test_LDADD = $(lib_LTLIBRARIES)
bin_PROGRAMS = iz_test
