PROG=	hid

ifndef VUSB
SRCS+=	khid.c
else
TARGET=	host
SRCS+=	vhid.c
endif

SRCS+=	hid.desc

include ../../build/mchck.mk
