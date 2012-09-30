CC=gcc
RM=rm
CFLAGS+=-I.
LFLAGS+=
DEPS=$(wildcard *.h)

.PHONY: all clean

all: wmbus-repeater

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -Wall -c -o $@ $<

wmbus-repeater: main.o amber.o network.o wmbus.o wmbus_dvl.o timedbuff.o
	$(CC) -o $@ $^ $(LFLAGS)

clean:
	@$(RM) -f wmbus-repeater
	@$(RM) -f *.o
