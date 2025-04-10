CC := /home/alt/Downloads/buildroot-2015.05/output/host/usr/bin/mips-buildroot-linux-uclibc-gcc
CFLAGS := -g -std=c99 -Wall -D_POSIX_C_SOURCE=200809L 

# CC := gcc 
# CFLAGS := -g -std=c99 -Wall -D_POSIX_C_SOURCE=200809L


PLUS_DIR := .

BIN_DIR := $(PLUS_DIR)/bin
OBJ_DIR := $(PLUS_DIR)/obj
SRC_DIR := $(PLUS_DIR)/src
DATA_DIR := $(PLUS_DIR)/database




SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))


all: $(OBJ_DIR) $(BIN_DIR) $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $(BIN_DIR)/get_info -lm


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ -lm

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rf $(BIN_DIR)/*
	rm -rf $(OBJ_DIR)/*