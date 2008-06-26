
all: user_guide.sgml developer_manual.sgml
	sgmltools -b onehtml user_guide.sgml
	dblatex  -o user_guide.pdf user_guide.sgml
	sgmltools -b onehtml developer_manual.sgml
	dblatex  -o developer_manual.pdf developer_manual.sgml
	

clean:
	rm -f developer_guide.pdf developer_manual.html *.html *.htm
	rm -rf html latex
