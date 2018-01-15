ACLOCAL_AMFLAGS = -I m4
SUBDIRS = src doc
EXTRA_DIST = bootstrap.sh COPYING README.md
AUTOMAKE_OPTIONS = foreign

# Refer to doc as $(top_builddir)/doc. Otherwise, cd cannot step to doc when
# CDPATH is set and has no dot.
fstab:
	@cd $(top_builddir)/src && $(MAKE) $@

libtext.html:
	@cd $(top_builddir)/doc && $(MAKE) $@
