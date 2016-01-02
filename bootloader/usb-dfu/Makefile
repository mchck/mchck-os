PROG=	dfu

CFLAGS+= -DSHORT_ISR -DSAVE_SPACE_AT_ALL_COST
USE=	usb
#NO_LTO= 1
LOADER=	1
DFUPID?=	0002		# bootloader-update-dfu is 0002

ifndef VUSB
SRCS+=	main-kinetis.c
else
TARGET=	host
SRCS+=	vdfu.c
endif

SRCS+=	dfu.desc

include ../../build/mchck.mk
