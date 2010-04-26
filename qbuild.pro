TEMPLATE=app
TARGET=om

CONFIG+=qtopia
DEFINES+=QTOPIA
DEFINES+=VERSION=0.10

# I18n info
STRING_LANGUAGE=en_US
LANGUAGES=en_US

# Package info
pkg [
    name=omhacks
    desc="Openmoko hardware control"
    license=GPLv2
    version=0.10
    maintainer="Radek Polak <psonek2@seznam.cz>"
]

SOURCES=\
    src/om.c \
    src/om-cmdline.c \
    omhacks/battery.c \
    omhacks/bt.c \
    omhacks/gps.c \
    omhacks/gsm.c \
    omhacks/led.c \
    omhacks/resumereason.c \
    omhacks/screen.c \
    omhacks/sysfs.c \
    omhacks/uevent.c \
    omhacks/usb.c \
    omhacks/wifi.c

# Install rules
target [
    hint=sxe
    domain=untrusted
]