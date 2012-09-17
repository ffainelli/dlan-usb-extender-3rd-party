CC=gcc
RM=rm
CFLAGS+=-I. -I$(KERNEL_SRC)/include 
LFLAGS+=
DEPS=
#CXXFLAGS+= -I. -I$(KERNEL_SRC)/include

.PHONY: all clean

all: wmbus-repeater

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -Wall -c -o $@ $<

%.o : %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

wmbus-repeater: main.o amber.o network.o wmbus.o wmbus_dvl.o timedbuff.o
	$(CC) -o $@ $^ $(LFLAGS)

clean:
	@$(RM) -f wmbus-repeater
	@$(RM) -f *.o
