all: master

# Compiler
CC = g++

# Warning Flags
CFLAGS = -std=c++11 -Werror -Wpedantic -Wall -Wextra -Wdouble-promotion -Wmissing-include-dirs -Wswitch-enum -Wunused -Wuninitialized

IFLAGS = -I../include/ -I../../02_05_Includes/

LFLAGS = -lSDL2 -lpthread

master: main.cpp display.o uart.o corner.o
	$(CC) main.cpp -o ../bin/console $(CFLAGS) $(IFLAGS) $(LFLAGS) ../build/display.o ../build/uart.o ../build/corner.o

display.o: display.cpp
	$(CC) -c display.cpp -o ../build/display.o $(CFLAGS) $(IFLAGS) $(LFLAGS)

uart.o: uart.cpp
	$(CC) -c uart.cpp -o ../build/uart.o $(CFLAGS) $(IFLAGS) $(LFLAGS)

corner.o: corner.cpp
	$(CC) -c corner.cpp -o ../build/corner.o $(CFLAGS) $(IFLAGS) $(LFLAGS)
