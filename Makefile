CC = gcc
CFLAGS = -Wall -pedantic -std=c99 -fPIC
LDFLAGS = -lrt -lpthread

all: libSocketUDP.a

libSocketUDP.a : SocketUDP.o SocketUDPUtils.o
	ar rcs $@ $^

clean :
	rm -f *.o libSocketUDP.so

install:
	cp *.a /usr/lib/ && cp *.h /usr/include/
 
