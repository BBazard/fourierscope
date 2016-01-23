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
export DOCDIR:=$(ROOT)/doc

export EXECNAME:=fourierscope
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
	mkdir -p build/logs
	printf "\033[0;34m"
	printf "Generating Documentation\n"
	doxygen $(DOCDIR)/config/doxygen.cfg
	cat $(BUILDDIR)/logs/docwarnings
	printf "\033[0m"
.PHONY: doc

clean:
	$(MAKE) -C $(RELEASEDIR) $@
	$(MAKE) -C $(TESTSDIR) $@
.PHONY: clean

mrproper:
	$(MAKE) -C $(RELEASEDIR) $@
	$(MAKE) -C $(TESTSDIR) $@
	printf "\033[0;33m"
	printf "Suppressing html and latex documentation\n"
	printf "\033[0m"
	-rm -r $(DOCDIR)/html $(DOCDIR)/latex 2> /dev/null
	-rm -rf $(BINDIR) $(BUILDDIR)
.PHONY: mrproper
