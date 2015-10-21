# Makefile

# Usage:
# ------
# make ledgerplot
# As root: make install
# make clean
# To remove:
# As root: make uninstall

CC = cc
CFLAGS = -g -O2 -W -Wall -Werror -Wextra
DEPS = ledgerplot.h
OBJ = ledgerplot.o
TARGET = ledgerplot

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(.ALLSRC)

$(OBJ): $(.PREFIX).c $(DEPS)
	$(CC) $(CFLAGS) -c $(.PREFIX).c
	
install:
	cp -v ledgerplot /usr/local/bin/
	chmod 755 /usr/local/bin/ledgerplot
	
uninstall:
	rm -v /usr/local/bin/ledgerplot
	
.PHONY: clean

clean:
	rm -rfv ledgerplot *.o
