# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror
LDFLAGS = -licuuc -licudata


# Build directory
BUILD_DIR = build

# Default target
all:
	@echo "Usage: make dayXX (e.g., make day01)"
	@echo "Make sure src/dayXX/dayXX.c exists."

# Rule to build and run dayXX
.PHONY: clean day01
day01: $(BUILD_DIR)/day01

# Rule to compile dayXX into the build directory
$(BUILD_DIR)/day01: src/day01/*.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

# Ensure the build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
	@echo "Build directory cleaned!"