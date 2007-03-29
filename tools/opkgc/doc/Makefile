all: opkgc-manual.sgml
	openjade -d /usr/share/sgml/docbook/stylesheet/dsssl/modular/html/docbook.dsl -t sgml opkgc-manual.sgml
	dblatex  -o opkgc-manual.pdf opkgc-manual.sgml
	sgmltools -b onehtml opkgc-manual.sgml
