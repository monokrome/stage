CFLAGS=
LDFLAGS=-lX11 -lXrandr

all: bin/stage

bin/%: %.c
	gcc $(CFLAGS) $(LDFLAGS) $< -o $@
