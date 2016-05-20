
include ../Makefile.common

CFLAGS	+= -I$(SBDEPS)/include/live555
LDFLAGS	+= -L$(SBDEPS)/lib -lliveMedia -lBasicUsageEnvironment -lUsageEnvironment -lgroupsock

OBJS	= server-live555.o sb-liveserver.o sb-mediasubsession.o sb-qossink.o \
	  sb-audiolivesource.o sb-videolivesource.o
TARGET	= server-live555.$(EXT)

include ../Makefile.build

