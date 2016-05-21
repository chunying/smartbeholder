
include ../Makefile.common

CFLAGS  += $(shell pkg-config --cflags opencv)
LDFLAGS += -I/usr/local/include/ -L/opt/vc/lib -I$(SBDEPS)/include/raspicam -L$(SBDEPS)/lib -lmmal -lmmal_core -lmmal_util
#-lraspicam -lraspicam_cv -L/opt/vc/lib -lmmal -lmmal_core -lmmal_util

OBJS	= vsource-stimage.o captst.o portPi.o

TARGET	= vsource-stimage.$(EXT)

include ../Makefile.build

