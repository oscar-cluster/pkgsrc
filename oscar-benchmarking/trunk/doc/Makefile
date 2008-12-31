LATEXFILES:=cbench

all: ${LATEXFILES}.tex
	for file in ${LATEXFILES} ; do ( latex $$file.tex; bibtex $$file; latex $$file.tex) ; done

clean:
	rm -f *~
	for file in ${LATEXFILES} ; do ( rm -f $$file.pdf  $$file.log $$file.aux $$file.bbl $$file.blg texput.log) ; done

