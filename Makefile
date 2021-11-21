# PolytreeWM - tiling window manager
# See LICENSE file for copyright and license details.

all: polytreewm

include config/1-generated.mk
include config/2-conditionals.mk
include config/3-defvars.mk

CONFIGMKS_TO_REMOVE = \
	config/1-generated.mk \
	config/2-conditionals.mk

CONFIGMKS = \
	$(CONFIGMKS_TO_REMOVE) \
	config/3-defvars.mk

VERSION = 6.2

MODULES_SRC = \
	src/atoms.c \
	src/constraints.c \
	src/drw.c \
	src/dwm.c \
	src/helpers.c \
	src/layouts.c \
	src/settings.c \
	src/spawn.c \
	src/state.c \
	src/unit.c \
	src/util.c

DWM_SRC = \
	src/dwm/handlers.c \
	src/dwm/layouts.c \
	src/dwm/xerror.c

MODULES_HDR = $(MODULES_SRC:.c=.h)
DWM_HDR = $(DWM_SRC:.c=.h)

SRC = $(MODULES_SRC) src/main.c
HDR = $(MODULES_HDR) src/config.def.h

OBJ = $(SRC:.c=.o)

polytreewm: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

dwm.o: $(DWM_SRC) $(DWM_HDR)
$(OBJ): $(CONFIGMKS) $(HDR)

clean:
	rm -f polytreewm $(OBJ)

distclean: clean
	rm -f $(CONFIGMKS_TO_REMOVE)

install: all
	mkdir -p $(DESTDIR)$(BINDIR)
	cp -f polytreewm $(DESTDIR)$(BINDIR)
	chmod 755 $(DESTDIR)$(BINDIR)/polytreewm
	
	mkdir -p $(DESTDIR)$(MANDIR)/man1
	sed "s/VERSION/$(VERSION)/g" < polytreewm.1 > $(DESTDIR)$(MANDIR)/man1/polytreewm.1
	chmod 644 $(DESTDIR)$(MANDIR)/man1/polytreewm.1

uninstall:
	rm -f \
		$(DESTDIR)$(BINDIR)/polytreewm \
		$(DESTDIR)$(MANDIR)/man1/polytreewm.1

.PHONY: all clean distclean install uninstall
