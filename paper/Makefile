SHARED=~/Dropbox/nodejs/web/buffer/problems.pdf
$(SHARED) : main.pdf
	cp main.pdf $(SHARED)
main.pdf : main.tex
	latex main.tex
	bibtex main.aux
	latex main.tex
	latex main.tex
	pdflatex main

