ACLOCAL_AMFLAGS = -I m4

INCLUDES = -I$(top_srcdir)

lib_LTLIBRARIES = libomhacks.la

sleephooksdir = $(libdir)/pm-utils/sleep.d/
sleephooks_LTLIBRARIES = omhacks.la

omhacksincludedir = $(includedir)/omhacks

omhacksinclude_HEADERS = \
	omhacks/all.h \
	omhacks/sysfs.h \
	omhacks/led.h \
	omhacks/screen.h \
	omhacks/resumereason.h

libomhacks_la_SOURCES = \
	omhacks/sysfs.c \
	omhacks/led.c \
	omhacks/screen.c \
	omhacks/resumereason.c

omhacks_la_SOURCES = hooks/omhacks.c
omhacks_la_LDFLAGS = -module
omhacks_la_LIBADD = libomhacks.la

bin_PROGRAMS = om om-led
sbin_PROGRAMS = pm-suspend

om_SOURCES = src/om.c src/om-cmdline.c
om_LDFLAGS = libomhacks.la

om_led_SOURCES = src/om-led.c src/om-cmdline.c
om_led_LDFLAGS = libomhacks.la

pm_suspend_SOURCES = src/pm-action.c
pm_suspend_LDFLAGS = -ldl -rdynamic
