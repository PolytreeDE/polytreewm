##########################
# Compiler, linker, etc. #
##########################

CC = cc

#########
# Paths #
#########

PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man

################
# Dependencies #
################

X11INC = /usr/X11R6/include
X11LIB = /usr/X11R6/lib

XINERAMALIBS  = -lXinerama

FREETYPELIBS = -lfontconfig -lXft
FREETYPEINC = /usr/include/freetype2
