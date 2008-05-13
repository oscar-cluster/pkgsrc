
all: user_guide.sgml developer_manual.sgml
#	openjade -d /usr/share/sgml/docbook/stylesheet/dsssl/modular/html/docbook.dsl -t sgml user_guide.sgml
	sgmltools -b onehtml user_guide.sgml
	dblatex  -o user_guide.pdf user_guide.sgml
#	openjade -d /usr/share/sgml/docbook/stylesheet/dsssl/modular/html/docbook.dsl -t sgml developer_manual.sgml
	sgmltools -b onehtml developer_manual.sgml
	dblatex  -o developer_manual.pdf developer_manual.sgml

clean:
	rm -f user_guide.pdf user_guide.html 
	rm -f developer_manual.pdf developer_manual.html
	rm -f *.html *.htm
	rm -rf html latex
