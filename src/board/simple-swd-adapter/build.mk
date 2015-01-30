SOC+=	kinetis/MKL26Z4
TARGET=	MKL26Z32VFM4

DFUVID?=	2323
DFUPID?=	0001

SRCS-board=	board.c
SRCS-board.dir= board/simple-swd-adapter
SRCS.libs+=	board
