TARGET.cortex-m+= m0
SOC+= kinetis

CFLAGS+=	-mcpu=cortex-m0 -msoft-float -mthumb

LDSCRIPTS+= $(call srcpath,src/soc/kinetis/MKL27Z4/MKL27Z4.ld)

ifneq ($(findstring 32,${TARGET}),)
RAM_SIZE=8192
FLASH_SIZE=32768
else
ifneq ($(findstring 64,${TARGET}),)
RAM_SIZE=16384
FLASH_SIZE=65536
else
ifneq ($(findstring 128,${TARGET}),)
RAM_SIZE=32768
FLASH_SIZE=131072
else
ifneq ($(findstring 256,${TARGET}),)
RAM_SIZE=32768
FLASH_SIZE=262144
endif
endif
endif
endif

ifneq ($(filter noboot,${TARGET_MODE}),)
LOADER=		yes
LOADER_SIZE=	${FLASH_SIZE}
else
LOADER_SIZE=	4096
endif

LOADER_ADDR=	0
APP_SIZE=	${FLASH_SIZE}-${LOADER_SIZE}
APP_ADDR=	${LOADER_SIZE}

SRCS-kinetis=	adc.c cop.c flash.c flashconfig.c ftfa.c fgpio.c gpio.c i2c.c lpuart.c pin.c pit.c rtc.c timeout.c uart.c uart-stdio.c usb.c sys-register.c
SRCS-kinetis.dir=	soc/kinetis
SRCS.libs+=	kinetis

SRCS.force-kinetis-MKL27Z4=	startup.c
SRCS-kinetis-MKL27Z4.dir=	soc/kinetis/MKL27Z4
SRCS.libs+=	kinetis-MKL27Z4

ifdef LOADER
SRCS.force-kinetis-MKL27Z4+=	flashconfig.c
endif
