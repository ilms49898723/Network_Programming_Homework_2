CC := g++

CXXFLAGS := -std=c++11 -I. -L. -Os -Wall -Wextra

.SUFFIXS:

.PHONY:
.PHONY: all client server clean

all: client server

client: client.cpp
	${CC} ${CXXFLAGS} -o $@ $<

server: server.cpp
	${CC} ${CXXFLAGS} -o $@ $<

clean:
	-rm -f *.o server client

