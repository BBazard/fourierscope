export CC:=gcc
export LD:=$(CC)

export CFLAGS+=-Wpedantic -std=c99
export LDFLAGS+=-ltiff -lfftw -lm

ROOT:=$(CURDIR)

export BINDIR:=$(ROOT)/bin
export BUILDIR:=$(ROOT)/build
export RELEASEDIR:=$(ROOT)/release
export TESTSDIR:=$(ROOT)/tests
export DOCDIR:=$(ROOT)/docs

export EXECNAME:="epics"
export TESTNAME:="runtests"

MAKE:=make -se

release:
CFLAGS+=-O2
	$(MAKE) -C $(RELEASEDIR)
.PHONY: release

tests:
CFLAGS+=-g
	$(MAKE) -C $(TESTSDIR)
.PHONY: tests

doc:
	doxygen $(DOCDIR)/config/config.cfg
.PHONY: doc

clean:
	$(MAKE) -C $(RELEASEDIR) $@
	$(MAKE) -C $(TESTSDIR) $@
.PHONY: clean

mrproper:
	$(MAKE) -C $(RELEASEDIR) $@
	$(MAKE) -C $(TESTSDIR) $@
.PHONY: mrproper
