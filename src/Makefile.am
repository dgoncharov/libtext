# The library name to build.
lib_LTLIBRARIES = libtext.la

# The list of header files that belong to the library.
libtext_la_HEADERS = libtext.h

# Where to install the headers on the system.
libtext_ladir = $(includedir)/libtext

# The sources to add to the library and to add to the distribution.
libtext_la_SOURCES = $(libtext_la_HEADERS) libtext.cpp

# The order of parameters to -version-info is current:revision:age.
# The library name on linux is libtext.so.(current - age).age.revision.
# gnu.org/software/libtool/manual/libtool.html#Libtool-versioning.
libtext_la_LDFLAGS = -version-info 1:0:0 -no-undefined

check_PROGRAMS = libtext.t
libtext_t_SOURCES = libtext.t.cpp test.h
libtext_t_LDADD = libtext.la

EXTRA_PROGRAMS = fstab
fstab_SOURCES = fstab.cpp
fstab_LDADD = libtext.la