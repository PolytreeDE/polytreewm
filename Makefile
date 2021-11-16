# PolytreeWM - tiling window manager
# See LICENSE file for copyright and license details.

include config.mk

SRC = \
	src/atoms.c \
	src/drw.c \
	src/dwm.c \
	src/helpers.c \
	src/layouts.c \
	src/menu.c \
	src/settings.c \
	src/spawn.c \
	src/tags.c \
	src/util.c

HDR = \
	src/atoms.h \
	src/drw.h \
	src/config.def.h \
	src/helpers.h \
	src/layouts.h \
	src/menu.h \
	src/settings.h \
	src/spawn.h \
	src/tags.h \
	src/util.h

DWM_SRC = \
	src/dwm/handlers.c \
	src/dwm/layouts.c \
	src/dwm/swallow.c \
	src/dwm/systray.c

OBJ = ${SRC:.c=.o}
DWM_HDR = ${DWM_SRC:.c=.h}

all: options polytreewm

options:
	@echo PolytreeWM build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

polytreewm: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

%.o: %.c
	${CC} -c $< -o $@ ${CFLAGS}

dwm.o: ${DWM_SRC} ${DWM_HDR}
${OBJ}: config.mk ${HDR}

clean:
	rm -f polytreewm ${OBJ}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f polytreewm ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/polytreewm
	
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	sed "s/VERSION/${VERSION}/g" < polytreewm.1 > ${DESTDIR}${MANPREFIX}/man1/polytreewm.1
	chmod 644 ${DESTDIR}${MANPREFIX}/man1/polytreewm.1

uninstall:
	rm -f \
		${DESTDIR}${PREFIX}/bin/polytreewm \
		${DESTDIR}${MANPREFIX}/man1/polytreewm.1

.PHONY: all options clean install uninstall
