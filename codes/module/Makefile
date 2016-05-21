
include Makefile.common

TARGET	= server-live555 streamer-omx encoder-x264 vsource-image vsource-stimage

all:
	for t in $(TARGET); do make -C $$t || exit 1; done

install:
	-mkdir -p ../../bin/mod
	find . -name '*.$(EXT)' -exec cp -f {} ../../bin/mod \;

clean:
	for t in $(TARGET); do make -C $$t clean; done

