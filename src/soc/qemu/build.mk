SOC+=	cortex-m

CFLAGS+=	-mcpu=cortex-m4 -mfloat-abi=soft -mthumb

LDSCRIPTS+= $(call srcpath,src/soc/qemu/qemu.ld) #XXX thispath

SRCS.force-qemu= crt.c semihosting.c
SRCS-qemu.dir= soc/qemu
SRCS.libs+= qemu

run: ${PROG}.elf
	qemu-system-arm -cpu cortex-m4 -machine lm3s811evb -monitor null -serial null -semihosting -nographic -kernel ${PROG}.elf

run-debug: ${PROG}.elf
	qemu-system-arm -cpu cortex-m4 -machine lm3s811evb -monitor null -serial null -semihosting -nographic -kernel ${PROG}.elf -S -s & arm-none-eabi-gdb -ex "target remote:1234" ${PROG}.elf; kill %1
