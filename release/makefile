ifeq ($(MAKELEVEL), 0)
error:
	printf "\033[0;31m"
	printf "Error : this makefile must be called from another makefile\n"
	printf "\033[0m"
.PHONY: error
.SILENT: error
endif

CFLAGS+=-O2

OUTPUTDIR:=$(BUILDDIR)/release

SRC:=$(wildcard src/*.c)
OBJS:=$(patsubst src/%.c,$(OUTPUTDIR)/%.o,$(SRC))

IFLAGS:=-I$(CURDIR)

all: $(OBJS)
	printf "\033[0;32m"
	printf "Creating $(EXECNAME) binary file in: $(BINDIR)\n"
	printf "\033[0m"
	$(LD) $(CFLAGS) -o $(BINDIR)/$(EXECNAME) $(OBJS) $(LDFLAGS)
.PHONY : all

$(OUTPUTDIR)/%.o: src/%.c
	mkdir -p $(OUTPUTDIR)
	printf "\033[0;35m"
	printf "Creating object file $(@F)\n"
	printf "\033[0m"
	$(CC) $(IFLAGS) $(CFLAGS) -c $< -o $@

clean:
	printf "\033[0;33m"
	printf "Cleaning release directory\n"
	printf "\033[0m"
	-rm -rf $(OUTPUTDIR)
.PHONY: clean

mrproper: clean
	printf "\033[0;33m"
	printf "Suppressing $(EXECNAME) binary\n"
	printf "\033[0m"
	-rm -f $(BINDIR)/$(EXECNAME)
.PHONY: mrproper
