CC = gcc
CFLAGS = -Iinclude
CFLAGS += $(shell pkg-config --cflags glib-2.0)
CFLAGS += -Wall -Wextra -Wpedantic
CFLAGS += -Wfloat-equal -Wshadow -Wpointer-arith -Wcast-align -Wswitch-enum -Wswitch-default -Wunreachable-code
CFLAGS += -Wcast-qual
CFLAGS += -Wconversion

LINKER_FLAGS = $(shell pkg-config --libs glib-2.0)
DEBUG_CFLAGS = -DDEBUG

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

SRC_FILES = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/**/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))
TARGET = $(BIN_DIR)/compiler.out

BUILD_SUBDIRS := $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(sort $(dir $(SRC_FILES))))

all: $(TARGET)

debug: CFLAGS += $(DEBUG_CFLAGS)
debug: $(TARGET)

$(TARGET): $(OBJ_FILES) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LINKER_FLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_SUBDIRS)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
