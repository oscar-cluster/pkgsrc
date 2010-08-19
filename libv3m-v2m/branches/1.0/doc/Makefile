
all: user_guide.sgml developer_manual.sgml
	sgmltools -b onehtml user_guide.sgml
	dblatex  -o user_guide.pdf user_guide.sgml
	sgmltools -b onehtml developer_manual.sgml
	dblatex  -o developer_manual.pdf developer_manual.sgml
	
install:
	install -d -m755 $(DESTDIR)/usr/share/doc/libv3m/
	install *.pdf $(DESTDIR)/usr/share/doc/libv3m/
	cp -rf html $(DESTDIR)/usr/share/doc/libv3m/

clean:
	rm -f developer_guide.pdf developer_manual.html *.html *.htm
	rm -rf html latex
