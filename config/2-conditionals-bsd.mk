.if "$(ENABLE_XINERAMA)" == "yes"
CPPFLAGS += -DENABLE_XINERAMA
PKGS += xinerama
.endif

.if "$(WITH_LOCKER)" == "i3lock"
CPPFLAGS += -DWITH_LOCKER -DWITH_LOCKER_I3LOCK
CARGO_FEATURES += --features locker-i3lock
.endif
.if "$(WITH_LOCKER)" == "i3lock-color"
CPPFLAGS += -DWITH_LOCKER -DWITH_LOCKER_I3LOCK_COLOR
CARGO_FEATURES += --features locker-i3lock-color
.endif

.if "$(WITH_TERMINAL)" == "alacritty"
CPPFLAGS += -DWITH_TERMINAL -DWITH_TERMINAL_ALACRITTY
CARGO_FEATURES += --features term-alacritty
.endif
.if "$(WITH_TERMINAL)" == "gnome"
CPPFLAGS += -DWITH_TERMINAL -DWITH_TERMINAL_GNOME
CARGO_FEATURES += --features term-gnome
.endif
.if "$(WITH_TERMINAL)" == "st"
CPPFLAGS += -DWITH_TERMINAL -DWITH_TERMINAL_ST
CARGO_FEATURES += --features term-st
.endif
.if "$(WITH_TERMINAL)" == "xterm"
CPPFLAGS += -DWITH_TERMINAL -DWITH_TERMINAL_XTERM
CARGO_FEATURES += --features term-xterm
.endif
