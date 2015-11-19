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
DEPS = ledgerplot.h c_generic/functions.h modules/income_vs_expenses.h
OBJ = ledgerplot.o
TARGET = ledgerplot

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(.ALLSRC)

$(OBJ): $(.PREFIX).c $(DEPS)
	$(CC) $(CFLAGS) -c $(.PREFIX).c
	
install:
	cp -v $(TARGET) /usr/local/bin/
	chmod 755 /usr/local/bin/$(TARGET)
	
uninstall:
	rm -v /usr/local/bin/$(TARGET)
	
.PHONY: clean

clean:
	rm -rfv $(TARGET) *.o
