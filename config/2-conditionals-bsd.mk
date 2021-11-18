.if "$(ENABLE_XINERAMA)" == "yes"
CPPFLAGS += -DENABLE_XINERAMA
PKGS += xinerama
.endif

.if "$(WITH_TERMINAL)" == "ptterm"
CPPFLAGS += -DWITH_TERMINAL -DWITH_TERMINAL_PTTERM
.endif
.if "$(WITH_TERMINAL)" == "xterm"
CPPFLAGS += -DWITH_TERMINAL -DWITH_TERMINAL_XTERM
.endif
.if "$(WITH_TERMINAL)" == "gnome"
CPPFLAGS += -DWITH_TERMINAL -DWITH_TERMINAL_GNOME
.endif
