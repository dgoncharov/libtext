MAKEFLAGS+=-r
SHELL:=/bin/bash
.ONESHELL:
.SECONDEXPANSION:
osname:=$(shell uname -s)

# This makefile expects to find the sources in the parent directory.
srcdir:=..
vpath %.c $(srcdir)

target:=token.t.tsk
obj:=token.t.o
dfiles:=$(obj:.o=.d)
.SECONDARY: $(obj)


asan_flags:=-fsanitize=address -fsanitize=pointer-compare\
  -fsanitize=undefined -fsanitize=pointer-subtract
#TODO: upgrade compiler on mac
ifeq ($(osname), Linux)
asan_flags+=-fsanitize=leak
endif

all_ldflags:=$(asan_flags) $(LDFLAGS)
all: $(target)
$(target): $(obj)
	$(CC) -o $@ $(all_ldflags) $^

# no-omit-frame-pointer to have proper backtrace.
# no-common to let asan instrument global variables.
# The options are gcc specific.
# The expected format of the generated .d files is the one used by gcc.
all_cppflags:=-I$(srcdir) $(CPPFLAGS)
all_cflags:=-Wall -Wextra -Wno-error -ggdb -O0 -m64\
  -fno-omit-frame-pointer\
  -fno-common\
  $(asan_flags) $(CFLAGS)

$(obj): %.o: %.c %.d $$(file <%.d)
	$(CC) $(all_cppflags) $(all_cflags) -MMD -MF $*.td -o $@ -c $< || exit 1
	read obj src headers <$*.td; echo "$$headers" >$*.d || exit 1
	touch -c $@

$(dfiles):;
%.h:;

asanopts:=detect_stack_use_after_return=0:detect_invalid_pointer_pairs=2:abort_on_error=1:disable_coredump=0:unmap_shadow_on_exit=1
check:
	ASAN_OPTIONS=$(asanopts) ./$(target)

clean:
	rm -f $(target) $(obj) $(dfiles) $(obj:.o=.td)

print-%: force
	$(info $*=$($*))

.PHONY: all clean force check
$(srcdir)/makefile::;
