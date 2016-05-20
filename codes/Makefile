
# Makefile for POSIX-based OSes

TARGET	= libcore module server

.PHONY: $(TARGET)

all: $(TARGET)

libcore:
	make -C libcore

module: libcore
	make -C module

server: libcore
	make -C server/rpi
	make -C server/rpisw

install:
	mkdir -p ../bin
	make -C libcore install
	make -C module install
	make -C server/rpi install
	make -C server/rpisw install
	cp -fr config ../bin/

clean:
	make -C libcore clean
	make -C module clean
	make -C server/rpi clean
	make -C server/rpisw clean

