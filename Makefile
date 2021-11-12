# dwm - dynamic window manager
# See LICENSE file for copyright and license details.

include config.mk

SRC = datetime.c drw.c dwm.c util.c
OBJ = ${SRC:.c=.o}

all: options dwm

options:
	@echo dwm build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	${CC} -c ${CFLAGS} $<

${OBJ}: config.h config.mk

dwm: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f dwm ${OBJ} dwm-${VERSION}.tar.gz

dist: clean
	mkdir -p dwm-${VERSION}
	cp -R LICENSE Makefile README config.def.h config.mk\
		dwm.1 drw.h util.h ${SRC} dwm.png transient.c dwm-${VERSION}
	tar -cf dwm-${VERSION}.tar dwm-${VERSION}
	gzip dwm-${VERSION}.tar
	rm -rf dwm-${VERSION}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f dwm ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/dwm
	
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	sed "s/VERSION/${VERSION}/g" < dwm.1 > ${DESTDIR}${MANPREFIX}/man1/dwm.1
	chmod 644 ${DESTDIR}${MANPREFIX}/man1/dwm.1

xinstall: install
	mkdir -p ${DESTDIR}${ICONSPREFIX}
	cp -f dwm.png ${DESTDIR}${ICONSPREFIX}
	chmod 644 ${DESTDIR}${ICONSPREFIX}/dwm.png
	
	mkdir -p ${DESTDIR}${XSESSIONSPREFIX}
	cp -f dwm.desktop ${DESTDIR}${XSESSIONSPREFIX}
	chmod 644 ${DESTDIR}${XSESSIONSPREFIX}/dwm.desktop

uninstall:
	rm -f \
		${DESTDIR}${PREFIX}/bin/dwm \
		${DESTDIR}${MANPREFIX}/man1/dwm.1 \
		${DESTDIR}${ICONSPREFIX}/dwm.png \
		${DESTDIR}${XSESSIONSPREFIX}/dwm.desktop

.PHONY: all options clean dist install uninstall
