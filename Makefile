# PolytreeWM - tiling window manager
# See LICENSE file for copyright and license details.

all: polytreewm

include config/1-generated.mk
include config/2-conditionals.mk
include config/3-defvars.mk

CONFIGMKS = \
	config/1-generated.mk \
	config/2-conditionals.mk \
	config/3-defvars.mk

VERSION = 6.2

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

polytreewm: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

%.o: %.c
	${CC} -c $< -o $@ ${CFLAGS}

dwm.o: ${DWM_SRC} ${DWM_HDR}
${OBJ}: ${CONFIGMKS} ${HDR}

clean:
	rm -f polytreewm ${OBJ}

distclean: clean
	rm -f $(CONFIGMKS)

install: all
	mkdir -p ${DESTDIR}${BINDIR}
	cp -f polytreewm ${DESTDIR}${BINDIR}
	chmod 755 ${DESTDIR}${BINDIR}/polytreewm
	
	mkdir -p ${DESTDIR}${MANDIR}/man1
	sed "s/VERSION/${VERSION}/g" < polytreewm.1 > ${DESTDIR}${MANDIR}/man1/polytreewm.1
	chmod 644 ${DESTDIR}${MANDIR}/man1/polytreewm.1

uninstall:
	rm -f \
		${DESTDIR}${BINDIR}/polytreewm \
		${DESTDIR}${MANDIR}/man1/polytreewm.1

.PHONY: all clean distclean install uninstall
