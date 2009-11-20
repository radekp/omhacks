CFLAGS=-O2 -Wall -g

all: pm-action om

pm-action: pm-action.c

om: om.c libomhacks.o

clean:
	rm -f pm-action libomhacks.o om
