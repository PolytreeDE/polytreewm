PolytreeWM - tiling window manager
==================================

PolytreeWM is an extremely fast, small, and dynamic tiling window manager for X.

Requirements
------------

In order to build PolytreeWM you need the Xlib header files.

Installation
------------

    ./configure
    make clean all
    sudo make install

Running
-------

Add the following line to your `.xinitrc` to start PolytreeWM using `startx`:

    exec polytreewm

In order to connect PolytreeWM to a specific display, make sure that the
`DISPLAY` environment variable is set correctly, e.g.:

    DISPLAY=foo.bar:1 exec polytreewm

This will start PolytreeWM on display :1 of the host foo.bar.

Credits
-------

PolytreeWM is originally a fork of [dwm](https://dwm.suckless.org), but is very
different now. It also includes the following dwm patches:

* [alwayscenter](https://dwm.suckless.org/patches/alwayscenter)
* [fakefullscreen](https://dwm.suckless.org/patches/fakefullscreen)
* [focusonclick](https://dwm.suckless.org/patches/focusonclick)
* [movestack](https://dwm.suckless.org/patches/movestack)
* [nmaxmaster](https://dwm.suckless.org/patches/nmaxmaster)
* [resetnmaster](https://dwm.suckless.org/patches/resetnmaster)
* [smartborders](https://dwm.suckless.org/patches/smartborders)
