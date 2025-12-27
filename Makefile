CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2 -Iinclude
AR = ar
ARFLAGS = rcs

SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
EXAMPLES_DIR = examples

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
LIBRARY = $(BUILD_DIR)/libcanon.a

EXAMPLES = $(wildcard $(EXAMPLES_DIR)/*.c)
EXAMPLE_BINS = $(EXAMPLES:$(EXAMPLES_DIR)/%.c=$(BUILD_DIR)/%)

.PHONY: all clean examples install

all: $(LIBRARY)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBRARY): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

examples: $(LIBRARY) $(EXAMPLE_BINS)

$(BUILD_DIR)/%: $(EXAMPLES_DIR)/%.c $(LIBRARY)
	$(CC) $(CFLAGS) $< -L$(BUILD_DIR) -lcanon -o $@

clean:
	rm -rf $(BUILD_DIR)

install: $(LIBRARY)
	install -d /usr/local/lib
	install -m 644 $(LIBRARY) /usr/local/lib/
	install -d /usr/local/include/canon
	install -m 644 $(INC_DIR)/canon/*.h /usr/local/include/canon/
