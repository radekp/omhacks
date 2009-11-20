CFLAGS=-O2 -Wall -g

all: pm-action om om-led

pm-action: pm-action.c

om: om.c libomhacks.o om-cmdline.o

om-led: om-led.c libomhacks.o om-cmdline.o

clean:
	rm -f *.o pm-action om om-led
