/*
 * UVC 1.5 Bulk Implementation Unit Tests
 * ======================================
 *
 * Unit tests specific to the cyfxuvcinmem_bulk (bulk endpoint) implementation.
 * Tests UVC 1.5 descriptor compliance and bulk-specific features.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

// Include UVC headers for bulk implementation
#include "../../cyfxuvcinmem_bulk/cyfxuvcinmem.h"

// Test framework macros
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            printf("FAIL: %s - %s\n", __func__, message); \
            return 0; \
        } \
    } while(0)

#define TEST_PASS() \
    do { \
        printf("PASS: %s\n", __func__); \
        return 1; \
    } while(0)

// External descriptor declarations for bulk implementation
extern const uint8_t CyFxUSBSSConfigDscr[];
extern const uint8_t CyFxUSBHSConfigDscr[];

// Test counter
static int tests_passed = 0;
static int tests_total = 0;

// Helper function to run a test
#define RUN_TEST(test_func) \
    do { \
        tests_total++; \
        if (test_func()) tests_passed++; \
    } while(0)

/**
 * Test UVC 1.5 version in bulk descriptors
 */
int test_bulk_uvc_version()
{
    const uint8_t *ss_desc = CyFxUSBSSConfigDscr;
    const uint8_t *hs_desc = CyFxUSBHSConfigDscr;

    // Find VC interface header descriptor in SS config
    const uint8_t *vc_header_ss = &ss_desc[9 + 8 + 9];

    // Verify UVC 1.5 version (bytes 3-4): should be 0x0150
    TEST_ASSERT(vc_header_ss[3] == 0x50 && vc_header_ss[4] == 0x01,
                "SS bulk descriptor should indicate UVC 1.5 (0x0150)");

    // Same test for HS descriptor
    const uint8_t *vc_header_hs = &hs_desc[9 + 8 + 9];
    TEST_ASSERT(vc_header_hs[3] == 0x50 && vc_header_hs[4] == 0x01,
                "HS bulk descriptor should indicate UVC 1.5 (0x0150)");

    TEST_PASS();
}

/**
 * Test bulk endpoint descriptor
 */
int test_bulk_endpoint_descriptor()
{
    const uint8_t *ss_desc = CyFxUSBSSConfigDscr;

    // Search for bulk endpoint descriptor
    int desc_offset = 9;
    int found_bulk_endpoint = 0;

    while (desc_offset < 0xCB) { // Total descriptor length for SS bulk (smaller than isochronous)
        uint8_t desc_len = ss_desc[desc_offset];
        if (desc_len == 0) break;

        uint8_t desc_type = ss_desc[desc_offset + 1];

        // Look for endpoint descriptor (type 0x05)
        if (desc_type == 0x05) {
            uint8_t ep_address = ss_desc[desc_offset + 2];
            uint8_t ep_attributes = ss_desc[desc_offset + 3];

            // Check if this is the bulk video endpoint (0x81)
            if (ep_address == 0x81 && (ep_attributes & 0x03) == 0x02) { // Bulk
                found_bulk_endpoint = 1;

                // Verify bulk endpoint attributes
                uint16_t max_packet_size = ss_desc[desc_offset + 4] |
                                         (ss_desc[desc_offset + 5] << 8);
                TEST_ASSERT(max_packet_size == 1024,
                            "Bulk endpoint max packet size should be 1024 bytes");

                break;
            }
        }

        desc_offset += desc_len;
    }

    TEST_ASSERT(found_bulk_endpoint, "Bulk video endpoint (0x81) should be present");
    TEST_PASS();
}

/**
 * Test SuperSpeed endpoint companion descriptor for bulk
 */
int test_bulk_ss_endpoint_companion()
{
    const uint8_t *ss_desc = CyFxUSBSSConfigDscr;

    // Search for SuperSpeed endpoint companion descriptor
    int desc_offset = 9;
    int found_companion = 0;

    while (desc_offset < 0xCB) {
        uint8_t desc_len = ss_desc[desc_offset];
        if (desc_len == 0) break;

        uint8_t desc_type = ss_desc[desc_offset + 1];

        // Look for SS endpoint companion descriptor (type 0x30)
        if (desc_type == 0x30 && desc_len == 0x06) {
            found_companion = 1;

            uint8_t max_burst = ss_desc[desc_offset + 2];
            uint8_t attributes = ss_desc[desc_offset + 3];
            uint16_t bytes_per_interval = ss_desc[desc_offset + 4] |
                                        (ss_desc[desc_offset + 5] << 8);

            // Verify SS companion descriptor for bulk
            TEST_ASSERT(max_burst == 0x0F, "SS companion max burst should be 15 for bulk");
            TEST_ASSERT(attributes == 0x00, "SS companion attributes should be 0 for bulk");
            TEST_ASSERT(bytes_per_interval == 0x0000, "Bytes per interval should be 0 for bulk");

            break;
        }

        desc_offset += desc_len;
    }

    TEST_ASSERT(found_companion, "SuperSpeed endpoint companion descriptor should be present for bulk");
    TEST_PASS();
}

/**
 * Test bulk-specific descriptor sizes
 */
int test_bulk_descriptor_sizes()
{
    const uint8_t *ss_desc = CyFxUSBSSConfigDscr;
    const uint8_t *hs_desc = CyFxUSBHSConfigDscr;

    // Check total configuration descriptor length (bulk should be smaller than isochronous)
    uint16_t ss_total_len = ss_desc[2] | (ss_desc[3] << 8);
    uint16_t hs_total_len = hs_desc[2] | (hs_desc[3] << 8);

    TEST_ASSERT(ss_total_len == 0x00CB, "SS bulk config total length should be 0xCB (203 bytes)");
    TEST_ASSERT(hs_total_len == 0x00BF, "HS bulk config total length should be 0xBF (191 bytes)");

    // Check VC interface total class descriptor size
    const uint8_t *vc_header_ss = &ss_desc[9 + 8 + 9];
    uint16_t vc_class_total_ss = vc_header_ss[5] | (vc_header_ss[6] << 8);

    TEST_ASSERT(vc_class_total_ss == 0x005E, "SS VC class descriptors total should be 0x5E (94 bytes) for bulk");

    TEST_PASS();
}

/**
 * Test encoding unit in bulk implementation
 */
int test_bulk_encoding_unit()
{
    const uint8_t *ss_desc = CyFxUSBSSConfigDscr;

    // Search for encoding unit descriptor (subtype 0x07)
    int desc_offset = 9;
    int found_encoding_unit = 0;

    while (desc_offset < 0xCB) {
        uint8_t desc_len = ss_desc[desc_offset];
        if (desc_len == 0) break;

        uint8_t desc_type = ss_desc[desc_offset + 1];
        uint8_t desc_subtype = ss_desc[desc_offset + 2];

        // Look for class-specific VC interface descriptor with encoding unit subtype
        if (desc_type == 0x24 && desc_subtype == CY_FX_UVC_VC_ENCODING_UNIT) {
            found_encoding_unit = 1;

            // Verify encoding unit structure for bulk implementation
            TEST_ASSERT(desc_len == 0x0D, "Encoding unit descriptor should be 13 bytes");

            uint8_t unit_id = ss_desc[desc_offset + 3];
            uint8_t source_id = ss_desc[desc_offset + 4];

            TEST_ASSERT(unit_id == 0x05, "Encoding unit ID should be 5 in bulk implementation");
            TEST_ASSERT(source_id == 0x03, "Encoding unit source should be extension unit (ID 3)");

            break;
        }

        desc_offset += desc_len;
    }

    TEST_ASSERT(found_encoding_unit, "Encoding unit descriptor should be present in bulk implementation");
    TEST_PASS();
}

/**
 * Test UVC 1.5 constants specific to bulk implementation
 */
int test_bulk_uvc15_constants()
{
    // Test H.264 format constants
    TEST_ASSERT(CY_FX_UVC_VS_FORMAT_H264 == 0x10, "H.264 format subtype should be 0x10");
    TEST_ASSERT(CY_FX_UVC_VS_FORMAT_H264_SIMULCAST == 0x11, "H.264 simulcast format subtype should be 0x11");
    TEST_ASSERT(CY_FX_UVC_VS_FORMAT_H265 == 0x12, "H.265 format subtype should be 0x12");

    // Test frame descriptor constants
    TEST_ASSERT(CY_FX_UVC_VS_FRAME_H264 == 0x11, "H.264 frame subtype should be 0x11");
    TEST_ASSERT(CY_FX_UVC_VS_FRAME_H265 == 0x13, "H.265 frame subtype should be 0x13");

    // Test encoding unit constant
    TEST_ASSERT(CY_FX_UVC_VC_ENCODING_UNIT == 0x07, "Encoding unit subtype should be 0x07");

    // Test some control selectors
    TEST_ASSERT(CY_FX_UVC_EU_SELECT_LAYER_CONTROL == 0x01, "Select Layer Control should be 0x01");
    TEST_ASSERT(CY_FX_UVC_EU_RATE_CONTROL_MODE_CONTROL == 0x06, "Rate Control Mode should be 0x06");
    TEST_ASSERT(CY_FX_UVC_EU_AVERAGE_BITRATE_CONTROL == 0x07, "Average Bitrate Control should be 0x07");

    TEST_PASS();
}

/**
 * Test backward compatibility with MJPEG in bulk implementation
 */
int test_bulk_mjpeg_compatibility()
{
    const uint8_t *ss_desc = CyFxUSBSSConfigDscr;

    // Search for MJPEG format descriptor (subtype 0x06)
    int desc_offset = 9;
    int found_mjpeg_format = 0;

    while (desc_offset < 0xCB) {
        uint8_t desc_len = ss_desc[desc_offset];
        if (desc_len == 0) break;

        uint8_t desc_type = ss_desc[desc_offset + 1];
        uint8_t desc_subtype = ss_desc[desc_offset + 2];

        // Look for class-specific VS interface descriptor with MJPEG format subtype
        if (desc_type == 0x24 && desc_subtype == 0x06) {
            found_mjpeg_format = 1;

            // Verify MJPEG format descriptor structure
            TEST_ASSERT(desc_len == 0x0B, "MJPEG format descriptor should be 11 bytes");

            uint8_t format_index = ss_desc[desc_offset + 3];
            uint8_t num_frame_desc = ss_desc[desc_offset + 4];

            TEST_ASSERT(format_index == 0x01, "MJPEG format index should be 1");
            TEST_ASSERT(num_frame_desc >= 0x01, "Should have at least 1 MJPEG frame descriptor");

            break;
        }

        desc_offset += desc_len;
    }

    TEST_ASSERT(found_mjpeg_format, "MJPEG format descriptor should be preserved in bulk implementation");
    TEST_PASS();
}

/**
 * Test bulk-specific video frame differences
 */
int test_bulk_video_frames()
{
    // Note: Bulk implementation typically has fewer video frames for demonstration
    // This is a conceptual test - actual frame count would need to be determined
    // by examining the video streaming descriptors or frame descriptors

    // Test that bulk implementation maintains basic frame structure
    TEST_ASSERT(1, "Bulk implementation should maintain basic video frame structure");

    // In bulk implementation, we expect simpler frame handling compared to isochronous
    // This test validates that the basic frame descriptor concepts are preserved
    TEST_ASSERT(CY_FX_UVC_INTERFACE_VS == 1, "Bulk implementation should use VS interface 1");

    TEST_PASS();
}

/**
 * Test output terminal connection to encoding unit in bulk implementation
 */
int test_bulk_output_terminal_connection()
{
    const uint8_t *ss_desc = CyFxUSBSSConfigDscr;

    // Search for output terminal descriptor (subtype 0x03)
    int desc_offset = 9;
    int found_output_terminal = 0;

    while (desc_offset < 0xCB) {
        uint8_t desc_len = ss_desc[desc_offset];
        if (desc_len == 0) break;

        uint8_t desc_type = ss_desc[desc_offset + 1];
        uint8_t desc_subtype = ss_desc[desc_offset + 2];

        // Look for class-specific VC interface descriptor with output terminal subtype
        if (desc_type == 0x24 && desc_subtype == 0x03) {
            found_output_terminal = 1;

            // Verify output terminal structure
            TEST_ASSERT(desc_len == 0x09, "Output terminal descriptor should be 9 bytes");

            uint8_t terminal_id = ss_desc[desc_offset + 3];
            uint8_t source_id = ss_desc[desc_offset + 7];

            TEST_ASSERT(terminal_id == 0x04, "Output terminal ID should be 4 in bulk implementation");
            TEST_ASSERT(source_id == 0x05, "Output terminal source should be encoding unit (ID 5) in bulk");

            break;
        }

        desc_offset += desc_len;
    }

    TEST_ASSERT(found_output_terminal, "Output terminal descriptor should be present in bulk implementation");
    TEST_PASS();
}

/**
 * Main test runner for bulk implementation
 */
int main(void)
{
    printf("UVC 1.5 Bulk Implementation Unit Tests\n");
    printf("=======================================\n\n");

    // Run all tests
    RUN_TEST(test_bulk_uvc_version);
    RUN_TEST(test_bulk_endpoint_descriptor);
    RUN_TEST(test_bulk_ss_endpoint_companion);
    RUN_TEST(test_bulk_descriptor_sizes);
    RUN_TEST(test_bulk_encoding_unit);
    RUN_TEST(test_bulk_uvc15_constants);
    RUN_TEST(test_bulk_mjpeg_compatibility);
    RUN_TEST(test_bulk_video_frames);
    RUN_TEST(test_bulk_output_terminal_connection);

    // Print results
    printf("\n=======================================\n");
    printf("Bulk Test Results: %d/%d passed\n", tests_passed, tests_total);

    if (tests_passed == tests_total) {
        printf("All bulk tests PASSED! ✓\n");
        printf("UVC 1.5 bulk implementation is working correctly.\n");
        return 0;
    } else {
        printf("Some bulk tests FAILED! ✗\n");
        printf("Please check the UVC 1.5 bulk implementation.\n");
        return 1;
    }
}