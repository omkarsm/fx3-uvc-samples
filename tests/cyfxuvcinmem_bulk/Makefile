# UVC 1.5 Bulk Implementation Unit Tests Makefile
# ================================================

CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -I../..
LDFLAGS=

# Test targets
BULK_DESC_TARGET=test_bulk_descriptors
BULK_CTRL_TARGET=test_bulk_controls

# Source files
BULK_DESC_SOURCES=test_bulk_descriptors.c ../../cyfxuvcinmem_bulk/cyfxuvcdscr.c
BULK_CTRL_SOURCES=test_bulk_controls.c

# Object files
BULK_DESC_OBJECTS=$(BULK_DESC_SOURCES:.c=.o)
BULK_CTRL_OBJECTS=$(BULK_CTRL_SOURCES:.c=.o)

# Default target - build all tests
all: $(BULK_DESC_TARGET) $(BULK_CTRL_TARGET)

# Build descriptor tests
$(BULK_DESC_TARGET): $(BULK_DESC_OBJECTS)
	$(CC) $(BULK_DESC_OBJECTS) -o $(BULK_DESC_TARGET) $(LDFLAGS)

# Build control tests
$(BULK_CTRL_TARGET): $(BULK_CTRL_OBJECTS)
	$(CC) $(BULK_CTRL_OBJECTS) -o $(BULK_CTRL_TARGET) $(LDFLAGS)

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run descriptor tests
test-descriptors: $(BULK_DESC_TARGET)
	@echo "=== Running Bulk Descriptor Tests ==="
	./$(BULK_DESC_TARGET)
	@echo ""

# Run control tests
test-controls: $(BULK_CTRL_TARGET)
	@echo "=== Running Bulk Control Tests ==="
	./$(BULK_CTRL_TARGET)
	@echo ""

# Run all tests
test: test-descriptors test-controls
	@echo "=== All Bulk Tests Completed ==="

# Clean build artifacts
clean:
	rm -f $(BULK_DESC_OBJECTS) $(BULK_CTRL_OBJECTS)
	rm -f $(BULK_DESC_TARGET) $(BULK_CTRL_TARGET)

# Create coverage report (requires gcov)
coverage: CFLAGS += -fprofile-arcs -ftest-coverage
coverage: LDFLAGS += -lgcov
coverage: clean all
	./$(BULK_DESC_TARGET)
	./$(BULK_CTRL_TARGET)
	gcov $(BULK_DESC_SOURCES) $(BULK_CTRL_SOURCES)

# Help target
help:
	@echo "Available targets for Bulk Implementation Tests:"
	@echo "  all              - Build all test executables"
	@echo "  test-descriptors - Build and run descriptor tests"
	@echo "  test-controls    - Build and run control tests"
	@echo "  test             - Build and run all tests"
	@echo "  clean            - Remove build artifacts"
	@echo "  coverage         - Generate test coverage report"
	@echo "  help             - Show this help message"

.PHONY: all test test-descriptors test-controls clean coverage help