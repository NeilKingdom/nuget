include setup.mk

SRC = $(wildcard *.c)

DDIR = $(PROJ_DIR)/include
DEPS = $(patsubst %.c, $(DDIR)/%.h, $(SRC))

ODIR = $(PROJ_DIR)/obj
OBJ = $(patsubst %.c, $(ODIR)/%.o, $(SRC))

CC = gcc
DEBUG = -g #-Werror -DDEBUG
IDIR = -I$(DDIR)
CFLAGS += $(DEBUG) -DTEMP_DIR=\"$(PROJ_DIR)/tmp/\" -Wall -Wextra -O2
LDFLAGS += -lncurses -lm -lc
BIN := nuget

all: $(OBJ)
	$(CC) -o $(BIN) $^ $(LDFLAGS) $(CFLAGS)

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(IDIR) $(CFLAGS) 

.PHONY: clean
clean: 
	rm -f $(BIN) $(ODIR)/*.o
