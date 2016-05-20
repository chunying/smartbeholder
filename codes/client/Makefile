
include ../Makefile.def

CFLAGS	+= -I../libcore $(AVCCF) $(L5CF) $(SDLCF)
LDFLAGS	= $(EXTRALDFLAGS) -L../libcore -lcore $(AVCLD) $(SDLLD) $(L5LD) -Wl,-rpath,\$$ORIGIN

ifeq ($(OS), Linux)
CFLAGS	+= $(X11CF)
LDFLAGS	+= $(X11LD) -lrt -lpthread
endif

ifeq ($(OS), Darwin)
LDFLAGS	+= -framework Cocoa
endif

TARGET	= client

all: $(TARGET)

.cpp.o:
	$(CXX) -c -g $(CFLAGS) $<

client: sb-client.o rtspclient.o ctrl-sdl.o minih264.o minivp8.o qosreport.o
	$(CXX) -o $@ $^ $(LDFLAGS)

install: $(TARGET)
	mkdir -p ../../bin
	cp -f $(TARGET) ../../bin
	cp -f *.ttf ../../bin

clean:
	rm -f $(TARGET) *.o *~

