include setup.mk

SRC=$(wildcard *.c)

DDIR=$(PROJ_DIR)/include
DEPS=$(patsubst %.c, $(DDIR)/%.h, $(SRC))

ODIR=$(PROJ_DIR)/obj
OBJ=$(patsubst %.c, $(ODIR)/%.o, $(SRC))

CC=gcc
DEBUG=-g
CFLAGS+=$(DEBUG) -I$(DDIR) -Wall -Wextra -Werror
LDFLAGS+=-lncurses -lm
BIN=nuget

all: $(OBJ)
	$(CC) -o $(BIN) $^ $(LDFLAGS) $(CFLAGS)

$(OBJ): $(SRC) $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean: 
	rm -f $(BIN) $(ODIR)/*.o
