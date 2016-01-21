export CC:=gcc
export LD:=$(CC)
export CXX:=g++
export LDXX:=$(CXX)

ROOT:=$(CURDIR)

export CFLAGS += -Wpedantic -std=c99
export CXXFLAGS += -Wpedantic -std=c++11
export LDFLAGS += -ltiff -lfftw3 -lm

export BINDIR:=$(ROOT)/bin
export BUILDDIR:=$(ROOT)/build
export RELEASEDIR:=$(ROOT)/release
export TESTSDIR:=$(ROOT)/tests
export DOCDIR:=$(ROOT)/docs

export EXECNAME:=epics
export TESTNAME:=runtests

MAKE:=make -se

.SILENT:

release:
	mkdir -p $(BINDIR) $(BUILDDIR)
	$(MAKE) -C $(RELEASEDIR)
.PHONY: release

tests:
	mkdir -p $(BINDIR) $(BUILDDIR)
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
	rmdir $(BINDIR) $(BUILDDIR)
.PHONY: mrproper
