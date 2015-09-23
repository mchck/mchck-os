SOC+= kinetis

CFLAGS+=	-mcpu=cortex-m0 -msoft-float -mthumb

LDSCRIPTS+= $(call srcpath,src/soc/kinetis/MKL26Z4/MKL26Z4.ld)

ifneq ($(findstring 32,${TARGET}),)
RAM_SIZE=4096
FLASH_SIZE=32768
else
ifneq ($(findstring 128,${TARGET},)
RAM_SIZE=16384
FLASH_SIZE=131072
endif
endif

ifneq ($(filter noboot,${TARGET_MODE}),)
LOADER=		yes
LOADER_SIZE=	${FLASH_SIZE}
else
LOADER_SIZE=	3072
endif

LOADER_ADDR=	0
APP_SIZE=	${FLASH_SIZE}-${LOADER_SIZE}
APP_ADDR=	${LOADER_SIZE}

SRCS-kinetis=	adc.c cop.c flash.c flashconfig.c ftfa.c fgpio.c gpio.c i2c.c pin.c pin_change.c pit.c rtc.c spi.c stdio.c timeout.c uart.c usb.c sys-register.c
SRCS-kinetis.dir=	soc/kinetis
SRCS.libs+=	kinetis

SRCS.force-kinetis-MKL26Z4=	startup.c
SRCS-kinetis-MKL26Z4.dir=	soc/kinetis/MKL26Z4
SRCS.libs+=	kinetis-MKL26Z4

ifdef LOADER
SRCS.force-kinetis-MKL26Z4+=	flashconfig.c
endif
