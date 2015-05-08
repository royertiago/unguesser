docdir := $(dir $(lastword $(MAKEFILE_LIST)))

# Stripped slash directory
docsdir := $(patsubst %/,%,$(docdir))

pdf := $(docdir)relatorio.pdf

.PHONY: doc
all: doc
doc: $(pdf)

$(pdf): $(docdir)relatorio.tex
	cd $(docdir) && latexmk -pdf relatorio.tex

.PHONY: doc-mostlyclean
mostlyclean: doc-mostlyclean
doc-mostlyclean:
	cd $(docdir) && latexmk -c

.PHONY: doc-clean
clean: doc-clean
doc-clean:
	cd $(docdir) && latexmk -C
