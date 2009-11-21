CFLAGS=-O2 -Wall -g -I.
LDFLAGS=-ldl -rdynamic

all: src/pm-action src/om src/om-led testhook.so

src/pm-action: src/pm-action.c

src/om: src/om.c src/om-cmdline.o omhacks/libomhacks.so

src/om-led: src/om-led.c src/om-cmdline.o omhacks/libomhacks.so

omhacks/libomhacks.so: omhacks/omhacks.c omhacks/omhacks.h
	gcc --shared -fPIC -o $@ $^

testhook.so: testhook.c
	gcc --shared -fPIC -o $@ $^

clean:
	rm -f *.o pm-action om om-led
