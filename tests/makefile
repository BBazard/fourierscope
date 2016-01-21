ifeq ($(MAKELEVEL), 0)
error:
	printf "\033[0;31m"
	printf "Error : this makefile must be called from another makefile\n"
	printf "\033[0m"
.PHONY: error
.SILENT: error
endif

override CXXFLAGS += -g
override LDFLAGS += -lgtest

OUTPUTDIR:=$(BUILDDIR)/tests

RELEASESRC:=$(filter-out $(RELEASEDIR)/src/main.c, $(wildcard $(RELEASEDIR)/src/*.c))
TESTSRC:=$(wildcard src/*.cpp)
RELEASEOBJS:=$(patsubst $(RELEASEDIR)/src/%.c,$(OUTPUTDIR)/%_xx.o,$(RELEASESRC))
TESTOBJS:=$(patsubst src/%.cpp,$(OUTPUTDIR)/%.o,$(TESTSRC))

IFLAGS:=-I$(RELEASEDIR)

all: $(TESTOBJS) $(RELEASEOBJS)
	printf "\033[0;32m"
	printf "Creating $(TESTNAME) binary file in: $(BINDIR)\n"
	printf "\033[0m"
	$(LDXX) $(CXXFLAGS) -o $(BINDIR)/${TESTNAME} $(RELEASEOBJS) $(TESTOBJS) $(LDFLAGS)
.PHONY: all

$(OUTPUTDIR)/%.o: src/%.cpp
	mkdir -p $(OUTPUTDIR)
	printf "\033[0;35m"
	printf "Creating object file $(@F)\n"
	printf "\033[0m"
	$(CXX) $(IFLAGS) $(CXXFLAGS) -c $< -o $@

$(OUTPUTDIR)/%_xx.o: $(RELEASEDIR)/src/%.c
	printf "\033[0;35m"
	printf "Creating object file $(@F)\n"
	printf "\033[0m"
	$(CXX) $(IFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	printf "\033[0;33m"
	printf "Cleaning tests directory\n"
	printf "\033[0m"
	-rm -rf $(OUTPUTDIR)
.PHONY: clean

mrproper: clean
	printf "\033[0;33m"
	printf "Suppressing $(TESTNAME) binary\n"
	printf "\033[0m"
	-rm -f $(BINDIR)/$(TESTNAME)
.PHONY: mrproper
