
include ../Makefile.def

CFLAGS	+= $(AVCCF) $(SDLCF)
SO_EXT	= .so

ifeq ($(HAVE_X11), 1)
CFLAGS	+= -DHAVE_X11
endif

ifeq ($(OS), Linux)
CFLAGS	+= $(ASNDCF) $(X11CF)
LDFLAGS	+= $(EXTRALDFLAGS) $(AVCLD)
endif

ifeq ($(OS), Darwin)
SO_EXT	= .dylib
LDFLAGS	+= $(EXTRALDFLAGS) -install_name @executable_path/libcore$(SO_EXT) $(AVCLD) $(L5LD)
endif

TARGET	= libcore$(SO_EXT)

all: $(TARGET)

.cpp.o:
	$(CXX) -c -g $(CFLAGS) $<

OBJS =	core-common.o core-conf.o core-confvar.o core-module.o core-avcodec.o \
	core-crc.o \
	rtspconf.o dpipe.o vconverter.o \
	vsource.o asource.o encoder-common.o \
	controller.o ctrl-msg.o

libcore.a: $(OBJS)
	$(AR) rc $@ $^

libcore$(SO_EXT): $(OBJS)
	$(CXX) -shared -o $@ $^ $(LDFLAGS)

install:
	cp -f libcore$(SO_EXT) ../../bin/

clean:
	rm -f $(TARGET) *.o *~

