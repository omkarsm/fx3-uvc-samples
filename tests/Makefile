# UVC 1.5 Unit Tests Makefile
# ============================

CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -I..
LDFLAGS=
TARGET=test_uvc_descriptors
SOURCES=test_uvc_descriptors.c ../cyfxuvcinmem/cyfxuvcdscr.c
OBJECTS=$(SOURCES:.c=.o)

# Default target
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run tests
test: $(TARGET)
	./$(TARGET)

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)

# Create coverage report (requires gcov)
coverage: CFLAGS += -fprofile-arcs -ftest-coverage
coverage: LDFLAGS += -lgcov
coverage: clean $(TARGET)
	./$(TARGET)
	gcov $(SOURCES)

# Help target
help:
	@echo "Available targets:"
	@echo "  all      - Build the test executable"
	@echo "  test     - Build and run tests"
	@echo "  clean    - Remove build artifacts"
	@echo "  coverage - Generate test coverage report"
	@echo "  help     - Show this help message"

.PHONY: all test clean coverage help