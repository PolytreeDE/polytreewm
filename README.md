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

Add the following line to your .xinitrc to start PolytreeWM using startx:

    exec polytreewm

In order to connect PolytreeWM to a specific display, make sure that the DISPLAY
environment variable is set correctly, e.g.:

    DISPLAY=foo.bar:1 exec polytreewm

(This will start PolytreeWM on display :1 of the host foo.bar.)

Credits
-------

PolytreeWM is a fork of [dwm](https://dwm.suckless.org).

Applied patches
---------------

* alwayscenter
* fakefullscreen
* focusonclick
* hide_vacant_tags
* movestack
* nametag
* nmaxmaster
* pertag
* resetnmaster
* smartborders
* swallow

Interesting patches (not applied)
---------------------------------

* attachdirection
* current_desktop
* ewmhtags
* focusmaster
* focusmonmouse
* rotatestack
* stacker
