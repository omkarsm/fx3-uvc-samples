# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Repository Overview

This is a Cypress FX3 USB Video Class (UVC) examples repository containing firmware implementations for USB 3.0 video streaming. The codebase demonstrates two different video streaming approaches:

- **cyfxuvcinmem**: UVC implementation using isochronous endpoints with video data streamed from internal memory
- **cyfxuvcinmem_bulk**: UVC implementation using bulk endpoints with video data streamed from internal memory

## Build System

The project uses a hierarchical Makefile-based build system that integrates with the Cypress FX3 SDK:

### Root Level Build Commands

```bash
# Build all examples (default)
make

# Build specific example
make CYFXOPT=uvcinmem      # Build only cyfxuvcinmem
make CYFXOPT=uvcinmem_bulk # Build only cyfxuvcinmem_bulk

# Clean all examples
make clean

# Eclipse-specific targets
make eclipse_build  # Copy required startup files to subdirectories
make eclipse_clean  # Remove build artifacts and copied files
```

### Individual Project Build Commands

Within each example directory (`cyfxuvcinmem/` or `cyfxuvcinmem_bulk/`):

```bash
# Build the project
make compile

# Clean build artifacts
make clean
```

## Architecture

### Project Structure

Each UVC example follows this pattern:
- `cyfxuvcinmem.c` - Main application logic and USB event handlers
- `cyfxuvcinmem.h` - Application constants and configuration
- `cyfxuvcdscr.c` - USB descriptors (device, configuration, interface)
- `cyfxuvcvidframes.c` - Video frame data stored as constant arrays
- `cyfxtx.c` - USB transfer and DMA management (copied from SDK)

### Key Differences Between Examples

**cyfxuvcinmem (Isochronous)**:
- Uses isochronous endpoints (EP 0x83) for video streaming
- Configured for ISO transfers with multiple packets per microframe
- Supports USB 3.0 burst transfers
- Packet size: 1024 bytes with 3 packets per microframe

**cyfxuvcinmem_bulk (Bulk)**:
- Uses bulk endpoints (EP 0x81) for video streaming
- Single packet transfers
- Packet size: 1024 bytes
- Fewer video frames (2 vs 4) for demonstration

### USB Configuration

Both examples implement:
- USB Video Class (UVC) 1.0 specification
- Video Control Interface (interface 0)
- Video Streaming Interface (interface 1) 
- Standard UVC probe/commit control mechanism
- MJPEG video format support

## Development Environment

The build system expects:
- Cypress FX3 SDK installed with `FX3FWROOT` pointing to SDK root (../../..)
- ARM or GCC toolchain as configured in SDK
- The SDK's `fx3_build_config.mak` provides compiler and linker settings

## Important Notes

- Video frames are stored as static arrays in `cyfxuvcvidframes.c`
- Both examples loop through predefined video frames continuously
- The bulk version is simpler and recommended for initial understanding
- Full speed USB is not supported (high speed and super speed only)
- Contains FX3-specific workarounds for Hi-Speed ISO transfer PID handling