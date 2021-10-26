ODIR=../obj
_OBJ=nuget.o fileio.o actions.o display.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

DDIR=../deps
_DEPS=nuget.h fileio.h actions.h display.h
DEPS=$(patsubst %,$(DDIR)/%,$(_DEPS))

CC=gcc
CFLAGS=-g -ansi -Wall -Wextra -pedantic
LIBS=-lncurses -lm
EXE=Nuget

$(ODIR)/%.o: %.c $(DEPS) 
	$(CC) -c -o $@ $< $(CFLAGS) 

# main program
all: $(OBJ)
	$(CC) -o $(EXE) $^ $(LIBS) $(CFLAGS) 

.PHONY: clean

# remove object files and program
clean: 
	rm $(EXE) $(ODIR)/* 
