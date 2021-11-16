##########################
# Compiler, linker, etc. #
##########################

CC = cc
PKGCONFIG = pkg-config

#########
# Paths #
#########

PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man

################
# Dependencies #
################

PKGS = fontconfig freetype2 x11 x11-xcb xcb xcb-res xft xinerama

CPPFLAGS = -DXINERAMA
CFLAGS   = `$(PKGCONFIG) --cflags $(PKGS)`
LDFLAGS  = `$(PKGCONFIG) --libs   $(PKGS)`
