TARGET.cortex-m+= m4
SOC+= kinetis

CFLAGS+=	-mcpu=cortex-m4 -msoft-float -mthumb

LDSCRIPTS+= $(call srcpath,src/soc/kinetis/MK22F51212/MK22F51212.ld)

RAM_SIZE=131072
FLASH_SIZE=524288

ifneq ($(filter noboot,${TARGET_MODE}),)
LOADER=		yes
LOADER_SIZE=	${FLASH_SIZE}
else
LOADER_SIZE=	32768
endif

LOADER_ADDR=	0
APP_SIZE=	${FLASH_SIZE}-65536
APP_ADDR=	65536

SRCS-kinetis=	adc.c crc.c flashconfig.c ftfa.c ftm.c gpio.c i2c.c pin.c pin_change.c pit.c rtc.c dspi.c timeout.c uart.c uart-fifo.c usb.c wdog.c sys-register.c
SRCS-kinetis.dir=	soc/kinetis
SRCS.libs+=	kinetis

SRCS.force-kinetis-MK22F51212=	startup.c
SRCS-kinetis-MK22F51212.dir=	soc/kinetis/MK22F51212
SRCS.libs+=	kinetis-MK22F51212

ifdef LOADER
SRCS.force-kinetis-MK22F51212+=	flashconfig.c
endif
