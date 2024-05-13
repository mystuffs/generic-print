PREFIX = /usr/local
demo: demo.c
	$(CC) demo.c -o demo
print.3: print.3.ronn
	ronn --roff --organization='exebook/generic-print' print.3.ronn
install: print.3
	cp print.h $(DESTDIR)$(PREFIX)/include/
	cp print.3 $(DESTDIR)$(PREFIX)/share/man/man3/
uninstall:
	rm -f $(DESKTOP)$(PREFIX)/include/print.h
	rm -f $(DESKTOP)$(PREFIX)/share/man/man3/print.3
clean:
	rm -f demo

