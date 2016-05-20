
include ../Makefile.def

CFLAGS	+= -I../../libcore $(AVCCF) $(L5CF) $(SDLCF) \
	   -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT -DUSE_EXTERNAL_OMX -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST \
	   -DUSE_VCHIQ_ARM \
	   -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux

#LDFLAGS	= -rdynamic -L../../libcore -Wl,--whole-archive -lcore -Wl,--no-whole-archive $(AVCLD) $(SDLLD)
EXTRALDFLAGS += -L/opt/vc/lib -lopenmaxil -lbcm_host -lvcos -lvchiq_arm -lpthread -lrt
LDFLAGS	= $(EXTRALDFLAGS) -rdynamic -L../../libcore -lcore $(AVCLD) $(SDLLD) -Wl,-rpath,\$$ORIGIN

ifeq ($(OS), Linux)
CFLAGS	+= $(ASNDCF) $(X11CF)
LDFLAGS	+= $(ASNDLD) $(X11LD)
endif

ifeq ($(OS), Darwin)
#LDFLAGS	= -rdynamic -force_load ../../libcore/libcore.a $(AVCLD) $(SDLLD)
LDFLAGS	+= -framework Cocoa
endif

TARGET	= server-rpi

all: $(TARGET)

.cpp.o:
	$(CXX) -c -g $(CFLAGS) $<

server-rpi: server-rpi.o 
	$(CXX) -o $@ $^ $(LDFLAGS)

install: $(TARGET)
	mkdir -p ../../../bin
	cp -f $(TARGET) ../../../bin/

clean:
	rm -f $(TARGET) *.o *~

