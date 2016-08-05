PREFIX?=	/usr/local
BINDIR?=	${PREFIX}/bin

CFLAGS+=	-Wall -Weverything

PROG=	kbdled
SRCS=	kbdled.c

MAN=

.include <bsd.prog.mk>
