TARGET.cortex-m+= m0
SOC+= kinetis

CFLAGS+=	-mcpu=cortex-m0 -msoft-float -mthumb

LDSCRIPTS+= $(call srcpath,src/soc/kinetis/MKL03Z4/MKL03Z4.ld)

RAM_SIZE=2048
ifneq ($(findstring 32,${TARGET}),)
FLASH_SIZE=32768
endif

LOADER?=	yes
LOADER_SIZE?=	${FLASH_SIZE}
LOADER_ADDR?=	0

SRCS-kinetis=	adc.c cop.c flash.c flashconfig.c ftfa.c fgpio.c gpio.c i2c.c lpuart.c uart.c uart-stdio.c pin.c pin_change.c rtc.c spi.c timeout.c sys-register.c
SRCS-kinetis.dir=	soc/kinetis
SRCS.libs+=	kinetis

SRCS.force-kinetis-MKL03Z4=	startup.c
SRCS-kinetis-MKL03Z4.dir=	soc/kinetis/MKL03Z4
SRCS.libs+=	kinetis-MKL03Z4

SRCS.force-kinetis-MKL03Z4+=	flashconfig.c
