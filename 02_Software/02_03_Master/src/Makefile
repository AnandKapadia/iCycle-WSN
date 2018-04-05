all: master

# Compiler
CC = g++

# Warning Flags
CFLAGS = -std=c++11 -Werror -Wpedantic -Wall -Wextra -Wdouble-promotion -Wmissing-include-dirs -Wswitch-enum -Wunused -Wuninitialized

IFLAGS = -I../include/

LFLAGS = -lSDL2

master: main.cpp display.o xbee16A.o
	$(CC) main.cpp -o ../bin/console $(CFLAGS) $(IFLAGS) $(LFLAGS) ../build/display.o ../build/xbee16A.o

display.o: display.cpp
	$(CC) -c display.cpp -o ../build/display.o $(CFLAGS) $(IFLAGS) $(LFLAGS)

xbee16A.o: xbee16A.cpp
	$(CC) -c xbee16A.cpp -o ../build/xbee16A.o $(CFLAGS) $(IFLAGS) $(LFLAGS)
