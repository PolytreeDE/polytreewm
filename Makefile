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
	src/geom.c \
	src/helpers.c \
	src/layouts.c \
	src/logger.c \
	src/settings.c \
	src/spawn.c \
	src/state.c \
	src/unit.c \
	src/util.c \
	src/xbase.c

DWM_SRC = \
	src/dwm/handlers.c \
	src/dwm/layouts.c \
	src/dwm/xerror.c

TEST_SRC = \
	tests/geom_position.c \
	tests/geom_sizes.c

MAIN_SRC = $(MODULES_SRC) src/main.c

MODULES_HDR = $(MODULES_SRC:.c=.h)
DWM_HDR     = $(DWM_SRC:.c=.h)
MAIN_HDR    = $(MODULES_HDR) src/main.h src/config.def.h

MODULES_OBJ = $(MODULES_SRC:.c=.o)
TEST_OBJ    = $(TEST_SRC:.c=.o)
ALL_OBJ     = $(MODULES_OBJ) $(TEST_OBJ) tests/main.o src/main.o

TEST_EXE = $(TEST_SRC:.c=.test)
ALL_EXE  = polytreewm $(TEST_EXE)

###############
# Executables #
###############

polytreewm: src/main.o $(MODULES_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.test: %.o tests/main.o $(MODULES_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

################
# Object files #
################

%.o: %.c $(CONFIGMKS) $(HDR)
	$(CC) -c $< -o $@ $(CFLAGS)

dwm.o: $(DWM_SRC) $(DWM_HDR)

#########
# Tasks #
#########

test: $(TEST_EXE)
	@echo "$(TEST_EXE)" | awk '{ OFS="\n"; $$1=$$1 } 1' | /bin/sh

clean:
	rm -f $(ALL_OBJ) $(ALL_EXE)

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

.PHONY: all clean distclean install uninstall test
