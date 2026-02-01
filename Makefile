# xappkeys - intercepting per-app macro daemon
# See LICENSE file for copyright and license details.
.POSIX:

include config.mk

SRC = xappkeys.c
OBJ = $(SRC:.c=.o)

all: $(OUT)

.c.o:
	$(CC) $(USED_CFLAGS) -c $<

xappkeys.o: config.h

$(OUT): $(OBJ)
	$(LD) $(OBJ) $(USED_LDFLAGS) -o $@

clean:
	rm -f $(OBJ) $(OUT)

install: $(OUT)
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f $(OUT) $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/$(OUT)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(OUT)

.PHONY: all clean install