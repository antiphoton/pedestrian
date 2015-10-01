SHARED=~/Dropbox/buffer/main.pdf
$(SHARED) : main.pdf
	cp main.pdf $(SHARED)
main.pdf : main.tex
	latex main.tex
	bibtex main.aux
	latex main.tex
	latex main.tex
	dvipdf main

