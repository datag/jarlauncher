# Makefile
# mingw32-make

CC=gcc
WINDRES=windres
STRIP=strip

SRCDIR=src
RESDIR=res
BINDIR=bin

all: $(BINDIR)/JarLauncher.exe


$(BINDIR)/JarLauncher.exe: $(BINDIR)/JarLauncher.obj $(BINDIR)/resource.obj
	$(CC) -mwindows -o $(BINDIR)/JarLauncher.exe $(BINDIR)/JarLauncher.obj $(BINDIR)/resource.obj

$(BINDIR)/JarLauncher.obj: $(SRCDIR)/main.c $(SRCDIR)/config.h
	$(CC) -c -o $(BINDIR)/JarLauncher.obj $(SRCDIR)/main.c

$(BINDIR)/resource.obj:	$(RESDIR)/appicon.ico $(SRCDIR)/resource.rc $(SRCDIR)/config.h
	$(WINDRES) $(SRCDIR)/resource.rc $(BINDIR)/resource.obj

strip: $(BINDIR)/JarLauncher.exe
	$(STRIP) --strip-all $(BINDIR)/JarLauncher.exe

clean:
	rm $(BINDIR)/*

.PHONY: all clean strip

