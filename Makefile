# ledgerplot
# See LICENSE file for copyright and license details.

include config.mk

SRC = ledgerplot.c functions.c enum.c income_vs_expenses.c
OBJ = ${SRC:.c=.o}
VPATH = .:c_generic:modules

all: options ledgerplot

options:
	@echo ledgerplot build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

${OBJ}: config.h config.mk

config.h:
	@echo creating $@ from config.def.h
	@cp config.def.h $@

ledgerplot: ${OBJ}
	@echo CC -o $@ ${OBJ} ${LDFLAGS}
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo cleaning...
	@rm -fv ledgerplot ${OBJ} ledgerplot-${VERSION}.tar.gz

dist: clean
	@echo creating dist tarball
	@mkdir -p ledgerplot-${VERSION}
	@cp -R LICENSE.txt Makefile README.adoc config.def.h config.mk \
		ledgerplot.1 ledgerplot.h ${SRC} ledgerplot-${VERSION}
	@tar -cf ledgerplot-${VERSION}.tar ledgerplot-${VERSION}
	@gzip ledgerplot-${VERSION}.tar
	@rm -rf ledgerplot-${VERSION}

install: all
	@echo installing executable file to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f ledgerplot ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/ledgerplot
	@echo installing gnuplot scripts to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${SHARE}/ledgerplot
	@cp -rfv gnuplot ${DESTDIR}${SHARE}/ledgerplot/gnuplot
	@chmod 644 ${DESTDIR}${SHARE}/ledgerplot/gnuplot/*
	@echo installing manual page to ${DESTDIR}${MANPREFIX}/man1
	@mkdir -p ${DESTDIR}${MANPREFIX}/man1
	@sed "s/VERSION/${VERSION}/g" < ledgerplot.1 > ${DESTDIR}${MANPREFIX}/man1/ledgerplot.1
	@chmod 644 ${DESTDIR}${MANPREFIX}/man1/ledgerplot.1

uninstall:
	@echo removing executable file from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/ledgerplot
	@echo removing data in /usr/local/share from ${DESTDIR}${SHARE}/ledgerplot
	@rm -rf ${DESTDIR}${SHARE}/ledgerplot
	@echo removing manual page from ${DESTDIR}${MANPREFIX}/man1
	@rm -f ${DESTDIR}${MANPREFIX}/man1/ledgerplot.1

.PHONY: all options clean dist install uninstall
