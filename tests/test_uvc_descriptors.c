/*
 * UVC 1.5 Descriptor Unit Tests
 * =============================
 * 
 * Unit tests to verify UVC 1.5 descriptor compliance and functionality.
 * These tests validate descriptor structure, sizes, and UVC 1.5 specific features.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

// Include UVC headers
#include "../cyfxuvcinmem/cyfxuvcinmem.h"

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

// External descriptor declarations
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
 * Test UVC version in descriptors
 */
int test_uvc_version_upgrade()
{
    const uint8_t *ss_desc = CyFxUSBSSConfigDscr;
    const uint8_t *hs_desc = CyFxUSBHSConfigDscr;
    
    // Find VC interface header descriptor in SS config
    // Skip configuration descriptor (9 bytes), interface association (8 bytes), 
    // video control interface (9 bytes) to reach VC header
    const uint8_t *vc_header_ss = &ss_desc[9 + 8 + 9];
    
    // Verify descriptor size and type
    TEST_ASSERT(vc_header_ss[0] == 0x0D, "SS VC header descriptor size should be 13");
    TEST_ASSERT(vc_header_ss[1] == 0x24, "SS VC header should be class-specific");
    TEST_ASSERT(vc_header_ss[2] == 0x01, "SS VC header should be VC_HEADER subtype");
    
    // Check UVC version (bytes 3-4): should be 0x0150 for UVC 1.5
    TEST_ASSERT(vc_header_ss[3] == 0x50 && vc_header_ss[4] == 0x01, 
                "SS descriptor should indicate UVC 1.5 (0x0150)");
    
    // Same test for HS descriptor
    const uint8_t *vc_header_hs = &hs_desc[9 + 8 + 9];
    TEST_ASSERT(vc_header_hs[3] == 0x50 && vc_header_hs[4] == 0x01,
                "HS descriptor should indicate UVC 1.5 (0x0150)");
    
    TEST_PASS();
}

/**
 * Test encoding unit descriptor presence
 */
int test_encoding_unit_descriptor()
{
    const uint8_t *ss_desc = CyFxUSBSSConfigDscr;
    
    // Search for encoding unit descriptor (subtype 0x07)
    int desc_offset = 9; // Start after config descriptor
    int found_encoding_unit = 0;
    
    while (desc_offset < 0xE0) { // Total descriptor length
        uint8_t desc_len = ss_desc[desc_offset];
        if (desc_len == 0) break; // Invalid descriptor
        
        uint8_t desc_type = ss_desc[desc_offset + 1];
        uint8_t desc_subtype = ss_desc[desc_offset + 2];
        
        // Look for class-specific VC interface descriptor with encoding unit subtype
        if (desc_type == 0x24 && desc_subtype == CY_FX_UVC_VC_ENCODING_UNIT) {
            found_encoding_unit = 1;
            
            // Verify encoding unit structure
            TEST_ASSERT(desc_len == 0x0D, "Encoding unit descriptor should be 13 bytes");
            
            uint8_t unit_id = ss_desc[desc_offset + 3];
            uint8_t source_id = ss_desc[desc_offset + 4];
            
            TEST_ASSERT(unit_id == 0x05, "Encoding unit ID should be 5");
            TEST_ASSERT(source_id == 0x03, "Encoding unit source should be extension unit (ID 3)");
            
            break;
        }
        
        desc_offset += desc_len;
    }
    
    TEST_ASSERT(found_encoding_unit, "Encoding unit descriptor should be present");
    TEST_PASS();
}

/**
 * Test UVC 1.5 constants definitions
 */
int test_uvc15_constants()
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
    TEST_ASSERT(CY_FX_UVC_EU_PROFILE_TOOLSET_CONTROL == 0x02, "Profile Toolset Control should be 0x02");
    TEST_ASSERT(CY_FX_UVC_EU_RATE_CONTROL_MODE_CONTROL == 0x06, "Rate Control Mode should be 0x06");
    
    TEST_PASS();
}

/**
 * Test output terminal connection to encoding unit
 */
int test_output_terminal_connection()
{
    const uint8_t *ss_desc = CyFxUSBSSConfigDscr;
    
    // Search for output terminal descriptor (subtype 0x03)
    int desc_offset = 9; // Start after config descriptor
    int found_output_terminal = 0;
    
    while (desc_offset < 0xE0) { // Total descriptor length
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
            
            TEST_ASSERT(terminal_id == 0x04, "Output terminal ID should be 4");
            TEST_ASSERT(source_id == 0x05, "Output terminal source should be encoding unit (ID 5)");
            
            break;
        }
        
        desc_offset += desc_len;
    }
    
    TEST_ASSERT(found_output_terminal, "Output terminal descriptor should be present");
    TEST_PASS();
}

/**
 * Test descriptor size calculations
 */
int test_descriptor_sizes()
{
    const uint8_t *ss_desc = CyFxUSBSSConfigDscr;
    const uint8_t *hs_desc = CyFxUSBHSConfigDscr;
    
    // Check total configuration descriptor length (bytes 2-3)
    uint16_t ss_total_len = ss_desc[2] | (ss_desc[3] << 8);
    uint16_t hs_total_len = hs_desc[2] | (hs_desc[3] << 8);
    
    TEST_ASSERT(ss_total_len == 0x00E0, "SS config total length should be 0xE0");
    TEST_ASSERT(hs_total_len == 0x00D4, "HS config total length should be 0xD4");
    
    // Check VC interface total class descriptor size
    const uint8_t *vc_header_ss = &ss_desc[9 + 8 + 9];
    uint16_t vc_class_total_ss = vc_header_ss[5] | (vc_header_ss[6] << 8);
    
    TEST_ASSERT(vc_class_total_ss == 0x005E, "SS VC class descriptors total should be 0x5E");
    
    TEST_PASS();
}

/**
 * Test backward compatibility with MJPEG
 */
int test_mjpeg_compatibility()
{
    const uint8_t *ss_desc = CyFxUSBSSConfigDscr;
    
    // Search for MJPEG format descriptor (subtype 0x06)
    int desc_offset = 9;
    int found_mjpeg_format = 0;
    
    while (desc_offset < 0xE0) {
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
            TEST_ASSERT(num_frame_desc == 0x01, "Should have 1 MJPEG frame descriptor");
            
            break;
        }
        
        desc_offset += desc_len;
    }
    
    TEST_ASSERT(found_mjpeg_format, "MJPEG format descriptor should still be present for backward compatibility");
    TEST_PASS();
}

/**
 * Main test runner
 */
int main(void)
{
    printf("UVC 1.5 Descriptor Unit Tests\n");
    printf("==============================\n\n");
    
    // Run all tests
    RUN_TEST(test_uvc_version_upgrade);
    RUN_TEST(test_encoding_unit_descriptor);
    RUN_TEST(test_uvc15_constants);
    RUN_TEST(test_output_terminal_connection);
    RUN_TEST(test_descriptor_sizes);
    RUN_TEST(test_mjpeg_compatibility);
    
    // Print results
    printf("\n==============================\n");
    printf("Test Results: %d/%d passed\n", tests_passed, tests_total);
    
    if (tests_passed == tests_total) {
        printf("All tests PASSED! ✓\n");
        printf("UVC 1.5 upgrade is working correctly.\n");
        return 0;
    } else {
        printf("Some tests FAILED! ✗\n");
        printf("Please check the UVC 1.5 implementation.\n");
        return 1;
    }
}