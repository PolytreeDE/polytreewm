CC = cc
PKGCONFIG = pkg-config

PKGS += fontconfig freetype2 x11 x11-xcb xcb xcb-res xft

.if "$(ENABLE_GNOME_TERMINAL)" == "yes"
CPPFLAGS += -DENABLE_GNOME_TERMINAL
.endif

.if "$(ENABLE_XINERAMA)" == "yes"
CPPFLAGS += -DENABLE_XINERAMA
PKGS += xinerama
.endif

CFLAGS  += `$(PKGCONFIG) --cflags $(PKGS)`
LDFLAGS += `$(PKGCONFIG) --libs   $(PKGS)`

CPPFLAGS += -DVERSION=\"$(VERSION)\" -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS   += $(CPPFLAGS) -std=c99 -Os -pedantic -Wall -Wno-deprecated-declarations
