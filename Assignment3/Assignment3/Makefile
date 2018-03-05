.phony all:
all: diskinfo disklist diskget diskput

diskinfo: diskinfo.c disk.h
	gcc diskinfo.c -lhistory -o diskinfo

disklist: disklist.c disk.h
	gcc disklist.c -lhistory -o disklist

diskget: diskget.c disk.h
	gcc diskget.c -lhistory -o diskget

diskput: diskput.c disk.h
	gcc diskput.c -lhistory -o diskput

.PHONY clean:
clean:
	-rm -rf *.o *.exe
