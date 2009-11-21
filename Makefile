CFLAGS=-O2 -Wall -g -I.
LDFLAGS=-ldl -rdynamic
libomhacks_SRC = omhacks/sysfs.h omhacks/sysfs.c \
	         omhacks/led.h omhacks/led.c \
	         omhacks/screen.h omhacks/screen.c \
	         omhacks/resumereason.h omhacks/resumereason.c
libomhacks_OBJ = omhacks/sysfs.o omhacks/led.o omhacks/screen.o omhacks/resumereason.o

all: omhacks/libomhacks.so omhacks/libomhacks.a src/pm-action src/om src/om-led hooks/omhacks.so

src/pm-action: src/pm-action.c

src/om: src/om.c src/om-cmdline.o omhacks/libomhacks.a

src/om-led: src/om-led.c src/om-cmdline.o omhacks/libomhacks.a

omhacks/libomhacks.so: $(libomhacks_SRC)
	gcc --shared -fPIC $(CFLAGS) -o $@ $^

omhacks/libomhacks.a: $(libomhacks_OBJ)
	ar rcs $@ $^

hooks/omhacks.so: hooks/omhacks.c omhacks/libomhacks.so
	gcc --shared -fPIC $(CFLAGS) -o $@ $^

clean:
	rm -f omhacks/*.o omhacks/libomhacks.so omhacks/libomhacks.a
	rm -f src/*.o src/pm-action src/om src/om-led
	rm -f hooks/omhacks.so
