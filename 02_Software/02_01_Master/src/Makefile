# Files to be compiled with the project
OBJS = main.cpp

# Compiler
CC = g++

# Warning Flags
CFLAGS = -std=c++11 -Werror -Wpedantic -Wall -Wextra -Wdouble-promotion -Wmissing-include-dirs -Wswitch-enum -Wunused -Wuninitialized

IFLAGS = -I../include/

LFLAGS = -lSDL2

OBJ_NAME = ../bin/display

master: $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) $(LFLAGS) -o $(OBJ_NAME)
