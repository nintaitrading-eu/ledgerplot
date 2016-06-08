# ledgerplot version
VERSION = 0.1

# Customize below to fit your system

# paths
PREFIX = /usr/local
# Note: the below will be /share/man on linux.
MANPREFIX = ${PREFIX}/man
INCLUDE = /usr/src/include
SHARE = ${PREFIX}/share

# includes and libs
#INCS = -I${X11INC} -I${FREETYPEINC}
INCS = -I${INCLUDE}
#LIBS = -L${X11LIB} -lX11 ${XINERAMALIBS} ${FREETYPELIBS}
LIBS =

# flags
#CPPFLAGS = -D_BSD_SOURCE -D_POSIX_C_SOURCE=2 -DVERSION=\"${VERSION}\"
CPPFLAGS = -D_BSD_SOURCE -D_POSIX_C_SOURCE=2
# release
#CFLAGS   = -std=c11 -Wpedantic -Wall -Wno-deprecated-declarations -Os ${INCS} ${CPPFLAGS}
# debug
CFLAGS = -g -Wall -Werror -std=c11 -O2 ${INCS}

#LDFLAGS  = -s ${LIBS}

# Solaris
#CFLAGS = -fast ${INCS} -DVERSION=\"${VERSION}\"
#LDFLAGS = ${LIBS}

# compiler and linker
CC = clang
