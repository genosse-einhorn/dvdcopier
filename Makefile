CC=gcc
PKGCONFIG=pkg-config

CFLAGS=-Og -Wall -Wextra -std=gnu99 -D_FILE_OFFSET_BITS=64
CFLAGS_PKG=$$($(PKGCONFIG) --cflags dvdread)
LDFLAGS=
LDFLAGS_PKG=$$($(PKGCONFIG) --libs dvdread)

out/cmdtool: cmdtool.c
	$(CC) $(CFLAGS) $(CFLAGS_PKG) -o $@ $< $(LDFLAGS) $(LDFLAGS_PKG)

.PHONY: clean
clean:
	rm -f out/cmdtool
