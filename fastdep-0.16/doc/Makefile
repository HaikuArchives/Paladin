DSSSLBASE = /usr/share/sgml/docbook/dsssl-stylesheets-1.74b/

all: fastdep.html fastdep.pdf

fastdep.html: fastdep.docbook
	jade -t sgml -d $(DSSSLBASE)/html/docbook.dsl \
		fastdep.docbook
	cp book1.htm fastdep.html

fastdep.tex: fastdep.docbook
	jade -o fastdep.tex -t tex -d $(DSSSLBASE)/print/docbook.dsl \
		fastdep.docbook
	
fastdep.dvi: fastdep.tex
	jadetex fastdep.tex
	jadetex fastdep.tex
	jadetex fastdep.tex

fastdep.ps: fastdep.dvi
	dvips -o fastdep.ps fastdep.dvi

fastdep.pdf: fastdep.ps
	ps2pdf fastdep.ps fastdep.pdf

clean:
	rm -rf *.htm *.html *.dvi *.tex *.pdf *.ps *.log *.aux

distclean:
	rm -rf *.log *.aux *.tex
