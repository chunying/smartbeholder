
include ../Makefile.def

CFLAGS	+= -I../../libcore $(AVCCF) $(L5CF) $(SDLCF)

#LDFLAGS	= -rdynamic -L../../libcore -Wl,--whole-archive -lcore -Wl,--no-whole-archive $(AVCLD) $(SDLLD)
LDFLAGS	+= -rdynamic -L../../libcore -lcore $(AVCLD) $(SDLLD) #-Wl,-rpath,\$$ORIGIN

ifeq ($(OS), Linux)
CFLAGS	+= $(ASNDCF) $(X11CF)
LDFLAGS	+= $(ASNDLD) $(X11LD)
endif

ifeq ($(OS), Darwin)
#LDFLAGS	= -rdynamic -force_load ../../libcore/libcore.a $(AVCLD) $(SDLLD)
LDFLAGS	+= -framework Cocoa
endif

TARGET	= server-rpisw

all: $(TARGET)

.cpp.o:
	$(CXX) -c -g $(CFLAGS) $<

server-rpisw: server-rpisw.o 
	$(CXX) -o $@ $^ $(LDFLAGS)

install: $(TARGET)
	mkdir -p ../../../bin
	cp -f $(TARGET) ../../../bin/

clean:
	rm -f $(TARGET) *.o *~

