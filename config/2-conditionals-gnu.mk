ifeq (yes,$(ENABLE_XINERAMA))
CPPFLAGS += -DENABLE_XINERAMA
PKGS += xinerama
endif

ifeq (i3lock,$(WITH_LOCKER))
CPPFLAGS += -DWITH_LOCKER -DWITH_LOCKER_I3LOCK
endif
ifeq (i3lock-color,$(WITH_LOCKER))
CPPFLAGS += -DWITH_LOCKER -DWITH_LOCKER_I3LOCK_COLOR
endif

ifeq (alacritty,$(WITH_TERMINAL))
CPPFLAGS += -DWITH_TERMINAL -DWITH_TERMINAL_ALACRITTY
endif
ifeq (gnome,$(WITH_TERMINAL))
CPPFLAGS += -DWITH_TERMINAL -DWITH_TERMINAL_GNOME
endif
ifeq (st,$(WITH_TERMINAL))
CPPFLAGS += -DWITH_TERMINAL -DWITH_TERMINAL_ST
endif
ifeq (xterm,$(WITH_TERMINAL))
CPPFLAGS += -DWITH_TERMINAL -DWITH_TERMINAL_XTERM
endif
