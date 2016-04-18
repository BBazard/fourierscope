SRCDIR:=$(CURDIR)/src
OUTDIR:=$(CURDIR)/output
LOGDIR=$(OUTDIR)

SRC:=$(wildcard $(SRCDIR)/*.tex)
OBJS=$(patsubst $(SRCDIR)/%.tex,$(OUTDIR)/%.png,$(SRC))
OBJS+=$(patsubst $(SRCDIR)/%.tex,$(OUTDIR)/%_75.png,$(SRC))
OBJS+=$(patsubst $(SRCDIR)/%.tex,$(OUTDIR)/%_50.png,$(SRC))
OBJS+=$(patsubst $(SRCDIR)/%.tex,$(OUTDIR)/%_25.png,$(SRC))
TMP=$(patsubst $(SRCDIR)/%.tex,$(OUTDIR)/%.log,$(SRC))
TMP+=$(patsubst $(SRCDIR)/%.tex,$(OUTDIR)/%.aux,$(SRC))

.SILENT:

all: $(OBJS)
	printf "Done generating latex/tikz images\n"
.PHONY: all

$(OUTDIR)/%.png: $(OUTDIR)/%.pdf
	printf "Convert pdf output to png\n"
	convert -density 300 $< -quality 90 $@

$(OUTDIR)/%_75.png: $(OUTDIR)/%.pdf
	convert -density 300 $< -quality 90 -resize 75% $@

$(OUTDIR)/%_50.png: $(OUTDIR)/%.pdf
	convert -density 300 $< -quality 90 -resize 50% $@

$(OUTDIR)/%_25.png: $(OUTDIR)/%.pdf
	convert -density 300 $< -quality 90 -resize 25% $@

$(OUTDIR)/%.pdf: $(SRCDIR)/%.tex
	printf "Compile pdf from tex\n"
	pdflatex -interaction=nonstopmode -halt-on-error -output-directory $(@D) $< > $(LOGDIR)/pdflatex.log

clean:
	printf "Cleaning temporary files\n"
	-rm $(LOGDIR)/pdflatex.log 2> /dev/null
	-rm $(TMP) 2> /dev/null
.PHONY: clean

mrproper: clean
	printf "Cleaning output files\n"
	-rm $(OBJS) 2> /dev/null
.PHONY: mrproper
