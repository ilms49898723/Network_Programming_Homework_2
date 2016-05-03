CC := g++

CXXFLAGS := -std=c++11 -I. -L. -Os -Wall -Wextra

OBJS := client.o \
		server.o \
		nputility.o

DEP := nputility.o

.SUFFIXS:
.SUFFIXS: .cpp .o

.PHONY:
.PHONY: all client server clean

all: client server

client: client.o ${DEP}
	${CC} ${CXXFLAGS} -o $@ $< ${DEP}

server: server.o ${DEP}
	${CC} ${CXXFLAGS} -o $@ $< ${DEP}

.cpp.o:
	${CC} ${CXXFLAGS} -c $<

clean:
	-rm -f *.o server client

