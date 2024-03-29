all: polytreewm

VERSION = 6.2

include config/1-generated.mk
include config/2-conditionals.mk
include config/3-defvars.mk

CONFIGMKS_TO_REMOVE = \
	config/1-generated.mk \
	config/2-conditionals.mk

CONFIGMKS = \
	$(CONFIGMKS_TO_REMOVE) \
	config/3-defvars.mk

RUST_SRC = \
	rust-polytreewm/Cargo.toml \
	rust-polytreewm/src/*.rs \
	rust-polytreewm/src/**/*.rs

RUST_APIS = \
	src/atoms.h \
	src/constraints.h \
	src/geom.h \
	src/helpers.h \
	src/settings.h

MODULES_SRC = \
	src/config/keys.c \
	src/config/buttons.c \
	src/config/layouts.c \
	src/drw.c \
	src/dwm.c \
	src/geom.c \
	src/logger.c \
	src/spawn.c \
	src/state.c \
	src/unit.c \
	src/util.c \
	src/xbase.c

DWM_SRC = \
	src/dwm/spaghetti/bar.c \
	src/dwm/spaghetti/handlers.c \
	src/dwm/spaghetti/layouts.c \
	src/dwm/spaghetti/wmcheckwin.c \
	src/dwm/spaghetti/xerror.c

TEST_SRC = \
	tests/geom_basic_geom.c \
	tests/geom_position.c \
	tests/geom_sizes.c \
	tests/geom_win_geom.c

MAIN_SRC = $(MODULES_SRC) src/main.c

MODULES_HDR = $(MODULES_SRC:.c=.h) $(RUST_APIS) src/config/common.h
DWM_HDR     = $(DWM_SRC:.c=.h) src/dwm/types.h
MAIN_HDR    = $(MODULES_HDR) src/main.h

MODULES_OBJ = $(MODULES_SRC:.c=.o)
TEST_OBJ    = $(TEST_SRC:.c=.o)
ALL_OBJ     = $(MODULES_OBJ) $(TEST_OBJ) tests/main.o src/main.o

TEST_EXE = $(TEST_SRC:.c=.test)
ALL_EXE  = polytreewm $(TEST_EXE)

###############
# Executables #
###############

polytreewm: src/main.o $(MODULES_OBJ) target/debug/libpolytreewm.a
	$(CC) -o $@ $^ $(LDFLAGS)

%.test: %.o tests/main.o $(MODULES_OBJ) target/debug/libpolytreewm.a
	$(CC) -o $@ $^ $(LDFLAGS)

################
# Object files #
################

%.o: %.c $(CONFIGMKS) $(HDR)
	$(CC) -c $< -o $@ $(CFLAGS)

dwm.o: $(DWM_SRC) $(DWM_HDR)

target/debug/libpolytreewm.a: $(RUST_SRC)
	$(CARGO) build

#########
# Tasks #
#########

test: $(TEST_EXE)
	@echo "$(TEST_EXE)" | awk '{ OFS="\n"; $$1=$$1 } 1' | /bin/sh
	$(CARGO) test
	$(CARGO) fmt --check
	$(CARGO) clippy

clean:
	rm -f $(ALL_OBJ) $(ALL_EXE)
	$(CARGO) clean

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
