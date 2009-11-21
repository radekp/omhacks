CFLAGS=-O2 -Wall -g
LDFLAGS=-ldl -rdynamic

all: pm-action om om-led testhook.so

pm-action: pm-action.c

om: om.c libomhacks.o om-cmdline.o

om-led: om-led.c libomhacks.o om-cmdline.o

testhook.so: testhook.c
	gcc --shared -fPIC -o $@ $^

clean:
	rm -f *.o pm-action om om-led
