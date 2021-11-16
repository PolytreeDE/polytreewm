# PolytreeWM - tiling window manager
# See LICENSE file for copyright and license details.

include config.mk

SRC = atoms.c drw.c dwm.c helpers.c layouts.c settings.c spawn.c tags.c util.c
OBJ = ${SRC:.c=.o}

DWM_SRC = dwm/handlers.c dwm/layouts.c dwm/swallow.c dwm/systray.c
DWM_HDR = ${DWM_SRC:.c=.h}

all: options polytreewm

options:
	@echo PolytreeWM build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

%.o: %.c
	${CC} -c $< -o $@ ${CFLAGS}

dwm.o: ${DWM_SRC} ${DWM_HDR}
${OBJ}: atoms.h drw.h config.def.h config.mk helpers.h layouts.h settings.h spawn.h tags.h util.h

polytreewm: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f polytreewm ${OBJ} polytreewm-${VERSION}.tar.gz

dist: clean
	mkdir -p polytreewm-${VERSION}
	cp -R LICENSE Makefile README.md config.def.h config.mk \
		polytreewm.1 drw.h util.h ${SRC} polytreewm.png polytreewm-${VERSION}
	tar -cf polytreewm-${VERSION}.tar polytreewm-${VERSION}
	gzip polytreewm-${VERSION}.tar
	rm -rf polytreewm-${VERSION}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f polytreewm ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/polytreewm
	
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	sed "s/VERSION/${VERSION}/g" < polytreewm.1 > ${DESTDIR}${MANPREFIX}/man1/polytreewm.1
	chmod 644 ${DESTDIR}${MANPREFIX}/man1/polytreewm.1

xinstall: install
	mkdir -p ${DESTDIR}${ICONSPREFIX}
	cp -f polytreewm.png ${DESTDIR}${ICONSPREFIX}
	chmod 644 ${DESTDIR}${ICONSPREFIX}/polytreewm.png
	
	mkdir -p ${DESTDIR}${XSESSIONSPREFIX}
	cp -f polytreewm.desktop ${DESTDIR}${XSESSIONSPREFIX}
	chmod 644 ${DESTDIR}${XSESSIONSPREFIX}/polytreewm.desktop

uninstall:
	rm -f \
		${DESTDIR}${PREFIX}/bin/polytreewm \
		${DESTDIR}${MANPREFIX}/man1/polytreewm.1 \
		${DESTDIR}${ICONSPREFIX}/polytreewm.png \
		${DESTDIR}${XSESSIONSPREFIX}/polytreewm.desktop

.PHONY: all options clean dist install uninstall xinstall
