MAKEFLAGS+=-r
SHELL:=/bin/bash
.ONESHELL:
.SECONDEXPANSION:
osname:=$(shell uname -s)

# This makefile expects to find the sources in the parent directory.
srcdir:=..
vpath %.c $(srcdir)
vpath %.c $(srcdir)/example

obj:=token.o token.t.o argv.o
dfiles:=$(obj:.o=.d)
.SECONDARY: $(obj)

BITNESS?=64
libdir:=/usr/lib
ifeq ($(BITNESS),64)
libdir:=/usr/lib64
else
libdir:=/usr/lib
endif
asan_flags:=-fsanitize=address -fsanitize=pointer-compare\
  -fsanitize=undefined -fsanitize=pointer-subtract
#TODO: upgrade compiler on mac
ifeq ($(osname), Linux)
asan_flags+=-fsanitize=leak
rpath:=-Wl,-rpath=$(CURDIR)
endif
ifeq ($(osname), SunOS)
asan_flags:=-fsanitize=undefined
rpath:=-Wl,-R$(CURDIR)
endif

all_ldflags:=-m$(BITNESS) $(asan_flags) $(rpath) $(LDFLAGS)

all: libtoken.so

libtoken.so: token.o
	$(CC) -o$@ -shared -fPIC $(all_ldflags) $^

# Do not link against libtoken, because token.t.c contains #include "token.c".
token.t.tsk: token.t.o
	$(CC) -o$@ $(all_ldflags) $^

argv.tsk: argv.o libtoken.so
	$(CC) $(all_ldflags) -o$@ $< -L. -ltoken

# no-omit-frame-pointer to have proper backtrace.
# no-common to let asan instrument global variables.
# The options are gcc specific.
# The expected format of the generated .d files is the one used by gcc.
all_cppflags:=-I$(srcdir) $(CPPFLAGS)
all_cflags:=-pipe -Wall -Wextra -Wfatal-errors\
  -Wwrite-strings -Wshadow\
  -Wdeclaration-after-statement -Wbad-function-cast -Wformat-security\
  -Wtype-limits -Wunused-but-set-parameter -Wlogical-op -Wpointer-arith\
  -Wignored-qualifiers -Wformat-signedness -Wduplicated-cond\
  -ggdb -O0 -m$(BITNESS)\
  -fno-default-inline\
  -fno-omit-frame-pointer\
  -fno-common\
  -fPIC\
  $(asan_flags) $(CFLAGS)

$(obj): %.o: %.c %.d $$(file <%.d)
	$(CC) $(all_cppflags) $(all_cflags) -MMD -MF $*.td -o $@ -c $< || exit 1
	read obj src headers <$*.td; echo "$$headers" >$*.d || exit 1
	touch -c $@

$(dfiles):;
%.h:;

asanopts:=detect_stack_use_after_return=0:detect_invalid_pointer_pairs=2:abort_on_error=1:disable_coredump=0:unmap_shadow_on_exit=1
check: token.t.tsk
	ASAN_OPTIONS=$(asanopts) ./token.t.tsk

clean:
	-rm -f token.t.tsk argv.tsk libtoken.so $(obj) $(dfiles) $(obj:.o=.td)

install: libtoken.so
	install -m 644 $(srcdir)/token.h /usr/include/ || exit 1
	install libtoken.so $(libdir)/libtoken.so.0.0.0 || exit 1
	ln -fs $(libdir)/libtoken.so.0.0.0 $(libdir)/libtoken.so.0 || exit 1
	ln -fs $(libdir)/libtoken.so.0.0.0 $(libdir)/libtoken.so || exit 1

uninstall:
	-rm -f /usr/include/token.h $(libdir)/libtoken.so.0.0.0\
          $(libdir)/libtoken.so $(libdir)/libtoken.so.0

print-%: force
	$(info $*=$($*))

.PHONY: all clean force check install uninstall
$(srcdir)/makefile::;
