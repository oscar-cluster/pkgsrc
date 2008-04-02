
all: html pdf

html: xoscar_manual.sgml
	sgmltools -b onehtml xoscar_manual.sgml

pdf: xoscar_manual.sgml
	dblatex  -o xoscar_manual.pdf xoscar_manual.sgml

clean:
	rm -f xoscar_manual.pdf xoscar_manual.html
	rm -f *.html *.htm *~
	rm -rf html latex
