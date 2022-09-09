ifeq (yes,$(ENABLE_XINERAMA))
CPPFLAGS += -DENABLE_XINERAMA
PKGS += xinerama
endif

ifeq (i3lock,$(WITH_LOCKER))
CPPFLAGS += -DWITH_LOCKER -DWITH_LOCKER_I3LOCK
CARGO_FEATURES += --features locker-i3lock
endif
ifeq (i3lock-color,$(WITH_LOCKER))
CPPFLAGS += -DWITH_LOCKER -DWITH_LOCKER_I3LOCK_COLOR
CARGO_FEATURES += --features locker-i3lock-color
endif

ifeq (alacritty,$(WITH_TERMINAL))
CPPFLAGS += -DWITH_TERMINAL -DWITH_TERMINAL_ALACRITTY
CARGO_FEATURES += --features term-alacritty
endif
ifeq (gnome,$(WITH_TERMINAL))
CPPFLAGS += -DWITH_TERMINAL -DWITH_TERMINAL_GNOME
CARGO_FEATURES += --features term-gnome
endif
ifeq (st,$(WITH_TERMINAL))
CPPFLAGS += -DWITH_TERMINAL -DWITH_TERMINAL_ST
CARGO_FEATURES += --features term-st
endif
ifeq (xterm,$(WITH_TERMINAL))
CPPFLAGS += -DWITH_TERMINAL -DWITH_TERMINAL_XTERM
CARGO_FEATURES += --features term-xterm
endif
