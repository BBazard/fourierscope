CC := gcc
LD := $(CC)
CXX := g++
LDXX := $(CXX)
CFLAGS += -Wall -Wextra -Wpedantic -std=c11 -O2 -pg
CXXFLAGS += -g -Wall -Wextra -Wpedantic -std=c++11 -pg
LDFLAGS += -ltiff -lfftw3 -lm
LINT:=cpplint --extensions=c,h,cpp
VALGRIND:=valgrind --leak-check=full --show-leak-kinds=all

BINDIR:=$(CURDIR)/bin
BUILDDIR:=$(CURDIR)/build
RELEASEDIR:=$(CURDIR)/release
TESTSDIR:=$(CURDIR)/tests
DOCDIR:=$(CURDIR)/doc
LOGDIR:=$(BUILDDIR)/logs

OUT:=$(BUILDDIR)/release
OUTTESTS:=$(BUILDDIR)/tests

SRC:=$(wildcard $(RELEASEDIR)/src/*.c)
INCLUDE:=$(wildcard $(RELEASEDIR)/include/*.h)
OBJS:=$(patsubst $(RELEASEDIR)/src/%.c,$(OUT)/%.o,$(SRC))

RELEASESRC:=$(filter-out $(RELEASEDIR)/src/main.c, $(wildcard $(RELEASEDIR)/src/*.c))
TESTSSRC:=$(wildcard $(TESTSDIR)/src/*.cpp)
RELEASEOBJS:=$(patsubst $(RELEASEDIR)/src/%.c,$(OUTTESTS)/%_xx.o,$(RELEASESRC))
TESTSOBJS:=$(patsubst $(TESTSDIR)/src/%.cpp,$(OUTTESTS)/%.o,$(TESTSSRC))

IFLAGS:=-I$(RELEASEDIR)

EXECNAME:=fourierscope
TESTSNAME:=runtests

.SILENT:

release: $(OBJS)
	mkdir -p $(BINDIR)
	printf "\033[0;32m"
	printf "Creating $(EXECNAME) binary file in: $(BINDIR)\n"
	printf "\033[0m"
	$(LD) $(CFLAGS) -o $(BINDIR)/$(EXECNAME) $(OBJS) $(LDFLAGS)

tests: LDFLAGS += -lgtest
tests: $(TESTSOBJS) $(RELEASEOBJS)
	mkdir -p $(BINDIR)
	printf "\033[0;32m"
	printf "Creating $(TESTSNAME) binary file in: $(BINDIR)\n"
	printf "\033[0m"
	$(LDXX) $(CXXFLAGS) -o $(BINDIR)/${TESTSNAME} $(RELEASEOBJS) $(TESTSOBJS) $(LDFLAGS)

$(OUTTESTS)/%.o: $(TESTSDIR)/src/%.cpp
	mkdir -p $(OUTTESTS)
	printf "\033[0;35m"
	printf "Creating object file $(@F)\n"
	printf "\033[0m"
	$(CXX) $(IFLAGS) $(CXXFLAGS) -c $< -o $@

$(OUTTESTS)/%_xx.o: $(RELEASEDIR)/src/%.c
	mkdir -p $(OUTTESTS)
	printf "\033[0;35m"
	printf "Creating object file $(@F)\n"
	printf "\033[0m"
	$(CXX) $(IFLAGS) $(CXXFLAGS) -c $< -o $@

$(OUT)/%.o: $(RELEASEDIR)/src/%.c
	mkdir -p $(OUT)
	printf "\033[0;35m"
	printf "Creating object file $(@F)\n"
	printf "\033[0m"
	$(CC) $(IFLAGS) $(CFLAGS) -c $< -o $@

doc:
	mkdir -p $(LOGDIR)
	printf "\033[0;34m"
	printf "Generating doc/images tikz images\n"
	make LOGDIR=$(LOGDIR) -C $(DOCDIR)/images
	printf "Generating Documentation\n"
	doxygen $(DOCDIR)/config/doxygen.cfg
	cat $(LOGDIR)/docwarnings
	printf "\033[0m"
.PHONY: doc

lint:
	printf "\033[0;36m"
	printf "Lint in progress\n"
	$(LINT) $(SRC) | true
	$(LINT) --root=$(CURDIR) $(INCLUDE) | true
	$(LINT) $(TESTSSRC) | true
	printf "\033[0m"
.PHONY: lint

valgrind:
	mkdir -p $(LOGDIR)
	printf "\033[0;36m"
	printf "Valgrind in progress\n"
	printf "\033[0m"
	-$(VALGRIND) $(BINDIR)/$(EXECNAME) 2> $(LOGDIR)/$(EXECNAME)_valgrind
	-$(VALGRIND) $(BINDIR)/$(TESTSNAME) 2> $(LOGDIR)/$(TESTSNAME)_valgrind
	printf "\033[0;36m"
	printf "Valgrind finished\n"
	printf "\033[0m"
.PHONY: valgrind

clean:
	printf "\033[0;33m"
	printf "Cleaning build directory\n"
	printf "\033[0m"
	-rm -r $(BUILDDIR) 2> /dev/null
.PHONY: clean

mrproper: clean
	printf "\033[0;33m"
	printf "Cleaning binary directory\n"
	printf "\033[0m"
	-rm -r $(BINDIR) 2> /dev/null
	printf "\033[0;33m"
	printf "Cleaning doc/images directory\n"
	make LOGDIR=$(LOGDIR) -C $(DOCDIR)/images $@
	printf "Cleaning doc directory\n"
	printf "\033[0m"
	-rm -r $(DOCDIR)/{html,latex} 2> /dev/null
.PHONY: mrproper
