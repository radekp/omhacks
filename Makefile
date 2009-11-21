CFLAGS=-O2 -Wall -g -I.
LDFLAGS=-ldl -rdynamic

all: src/pm-action src/om src/om-led testhook.so

src/pm-action: src/pm-action.c

src/om: src/om.c src/om-cmdline.o omhacks/libomhacks.so

src/om-led: src/om-led.c src/om-cmdline.o omhacks/libomhacks.so

omhacks/libomhacks.so: \
	omhacks/sysfs.h omhacks/sysfs.c \
	omhacks/led.h omhacks/led.c \
	omhacks/resumereason.h omhacks/resumereason.c
	gcc --shared -fPIC $(CFLAGS) -o $@ $^

testhook.so: testhook.c
	gcc --shared -fPIC $(CFLAGS) -o $@ $^

clean:
	rm -f omhacks/*.o src/*.o src/ src/{pm-action,om,om-led} omhacks/libomhacks.so
