
all: user_guide.sgml 
	openjade -d /usr/share/sgml/docbook/stylesheet/dsssl/modular/html/docbook.dsl -t sgml user_guide.sgml
	sgmltools -b onehtml user_guide.sgml
	dblatex  -o user_guide.pdf user_guide.sgml

clean:
	rm -f user_guide.pdf user_guide.html *.html *.htm
	rm -rf html latex
