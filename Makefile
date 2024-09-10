CC = gcc
PROFILE ?= DEBUG

CCFLAGS_DEBUG = -ggdb -O0 -fno-builtin -DDEBUG
CCLFLAGS_RELEASE = -O2

SRC_DIR := src
INC_DIR := include
OBJ_DIR := obj

SRCS := $(wildcard $(SRC_DIR)/*.c)
DEPS := $(wildcard $(INC_DIR)/*.h)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

CCFLAGS += $(CCFLAGS_$(PROFILE)) -I$(INC_DIR) -Wall -Wextra -Wformat #-Werror
LDFLAGS += -lc -lm -lcurses -lcsv

BIN := nuget

$(BIN): $(OBJS)
	$(CC) $^ -o $@ $(CCFLAGS) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $< -c -o $@ $(CCFLAGS)

rebuild: clean $(BIN)

clean:
	rm -f $(OBJS) $(BIN)

.PHONY: clean rebuild
