# UVC 1.5 Isochronous Implementation Unit Tests Makefile
# =====================================================

CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -I../..
LDFLAGS=

# Test targets
ISO_DESC_TARGET=test_iso_descriptors
ISO_CTRL_TARGET=test_iso_controls

# Source files
ISO_DESC_SOURCES=test_iso_descriptors.c ../../cyfxuvcinmem/cyfxuvcdscr.c
ISO_CTRL_SOURCES=test_iso_controls.c

# Object files
ISO_DESC_OBJECTS=$(ISO_DESC_SOURCES:.c=.o)
ISO_CTRL_OBJECTS=$(ISO_CTRL_SOURCES:.c=.o)

# Default target - build all tests
all: $(ISO_DESC_TARGET) $(ISO_CTRL_TARGET)

# Build descriptor tests
$(ISO_DESC_TARGET): $(ISO_DESC_OBJECTS)
	$(CC) $(ISO_DESC_OBJECTS) -o $(ISO_DESC_TARGET) $(LDFLAGS)

# Build control tests
$(ISO_CTRL_TARGET): $(ISO_CTRL_OBJECTS)
	$(CC) $(ISO_CTRL_OBJECTS) -o $(ISO_CTRL_TARGET) $(LDFLAGS)

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run descriptor tests
test-descriptors: $(ISO_DESC_TARGET)
	@echo "=== Running Isochronous Descriptor Tests ==="
	./$(ISO_DESC_TARGET)
	@echo ""

# Run control tests
test-controls: $(ISO_CTRL_TARGET)
	@echo "=== Running Isochronous Control Tests ==="
	./$(ISO_CTRL_TARGET)
	@echo ""

# Run all tests
test: test-descriptors test-controls
	@echo "=== All Isochronous Tests Completed ==="

# Clean build artifacts
clean:
	rm -f $(ISO_DESC_OBJECTS) $(ISO_CTRL_OBJECTS)
	rm -f $(ISO_DESC_TARGET) $(ISO_CTRL_TARGET)

# Create coverage report (requires gcov)
coverage: CFLAGS += -fprofile-arcs -ftest-coverage
coverage: LDFLAGS += -lgcov
coverage: clean all
	./$(ISO_DESC_TARGET)
	./$(ISO_CTRL_TARGET)
	gcov $(ISO_DESC_SOURCES) $(ISO_CTRL_SOURCES)

# Help target
help:
	@echo "Available targets for Isochronous Implementation Tests:"
	@echo "  all              - Build all test executables"
	@echo "  test-descriptors - Build and run descriptor tests"
	@echo "  test-controls    - Build and run control tests"
	@echo "  test             - Build and run all tests"
	@echo "  clean            - Remove build artifacts"
	@echo "  coverage         - Generate test coverage report"
	@echo "  help             - Show this help message"

.PHONY: all test test-descriptors test-controls clean coverage help