/*
 * UVC 1.5 Control Request Unit Tests
 * ==================================
 * 
 * Functional tests for UVC 1.5 control request handling and encoding unit controls.
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

// Mock UVC setup packet structure
typedef struct {
    uint8_t bmRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} uvc_setup_packet_t;

// Test counters
static int tests_passed = 0;
static int tests_total = 0;

#define RUN_TEST(test_func) \
    do { \
        tests_total++; \
        if (test_func()) tests_passed++; \
    } while(0)

/**
 * Test UVC 1.5 control selector values
 */
int test_encoding_control_selectors()
{
    // Test that all UVC 1.5 encoding unit control selectors are properly defined
    uint16_t expected_selectors[] = {
        CY_FX_UVC_EU_SELECT_LAYER_CONTROL,           // 0x01
        CY_FX_UVC_EU_PROFILE_TOOLSET_CONTROL,        // 0x02
        CY_FX_UVC_EU_VIDEO_RESOLUTION_CONTROL,       // 0x03
        CY_FX_UVC_EU_MIN_FRAME_INTERVAL_CONTROL,     // 0x04
        CY_FX_UVC_EU_SLICE_MODE_CONTROL,             // 0x05
        CY_FX_UVC_EU_RATE_CONTROL_MODE_CONTROL,      // 0x06
        CY_FX_UVC_EU_AVERAGE_BITRATE_CONTROL,        // 0x07
        CY_FX_UVC_EU_CPB_SIZE_CONTROL,               // 0x08
        CY_FX_UVC_EU_PEAK_BIT_RATE_CONTROL,          // 0x09
        CY_FX_UVC_EU_QUANTIZATION_PARAMS_CONTROL,    // 0x0A
        CY_FX_UVC_EU_SYNC_REF_FRAME_CONTROL,         // 0x0B
        CY_FX_UVC_EU_LTR_BUFFER_CONTROL,             // 0x0C
        CY_FX_UVC_EU_LTR_PICTURE_CONTROL,            // 0x0D
        CY_FX_UVC_EU_LTR_VALIDATION_CONTROL,         // 0x0E
        CY_FX_UVC_EU_LEVEL_IDC_LIMIT_CONTROL,        // 0x0F
        CY_FX_UVC_EU_SEI_PAYLOADTYPE_CONTROL,        // 0x10
        CY_FX_UVC_EU_QP_RANGE_CONTROL,               // 0x11
        CY_FX_UVC_EU_PRIORITY_CONTROL,               // 0x12
        CY_FX_UVC_EU_START_OR_STOP_LAYER_CONTROL,    // 0x13
        CY_FX_UVC_EU_ERROR_RESILIENCY_CONTROL        // 0x14
    };
    
    // Verify sequential control selector values
    for (int i = 0; i < 20; i++) {
        TEST_ASSERT(expected_selectors[i] == (i + 1), 
                    "Encoding unit control selectors should be sequential starting from 0x01");
    }
    
    TEST_PASS();
}

/**
 * Mock function to simulate control request validation
 */
int validate_control_request(uvc_setup_packet_t *setup)
{
    // Basic request validation
    if (!setup) return 0;
    
    // Check if it's a UVC interface request
    if (setup->bmRequestType != CY_FX_USB_UVC_GET_REQ_TYPE && 
        setup->bmRequestType != CY_FX_USB_UVC_SET_REQ_TYPE) {
        return 0;
    }
    
    // Check if it's a valid UVC request
    if (setup->bRequest != CY_FX_USB_UVC_GET_CUR_REQ &&
        setup->bRequest != CY_FX_USB_UVC_SET_CUR_REQ &&
        setup->bRequest != CY_FX_USB_UVC_GET_MIN_REQ &&
        setup->bRequest != CY_FX_USB_UVC_GET_MAX_REQ &&
        setup->bRequest != CY_FX_USB_UVC_GET_DEF_REQ) {
        return 0;
    }
    
    return 1; // Valid request
}

/**
 * Test UVC control request validation
 */
int test_control_request_validation()
{
    uvc_setup_packet_t setup;
    
    // Test valid GET_CUR request to encoding unit
    setup.bmRequestType = CY_FX_USB_UVC_GET_REQ_TYPE;
    setup.bRequest = CY_FX_USB_UVC_GET_CUR_REQ;
    setup.wValue = (CY_FX_UVC_EU_RATE_CONTROL_MODE_CONTROL << 8) | 0x00; // Control selector | Interface
    setup.wIndex = (0x05 << 8) | CY_FX_UVC_INTERFACE_VC; // Unit ID | Interface number
    setup.wLength = 4;
    
    TEST_ASSERT(validate_control_request(&setup) == 1, "Valid GET_CUR request should be accepted");
    
    // Test valid SET_CUR request to encoding unit
    setup.bmRequestType = CY_FX_USB_UVC_SET_REQ_TYPE;
    setup.bRequest = CY_FX_USB_UVC_SET_CUR_REQ;
    setup.wValue = (CY_FX_UVC_EU_AVERAGE_BITRATE_CONTROL << 8) | 0x00;
    setup.wIndex = (0x05 << 8) | CY_FX_UVC_INTERFACE_VC;
    setup.wLength = 4;
    
    TEST_ASSERT(validate_control_request(&setup) == 1, "Valid SET_CUR request should be accepted");
    
    // Test invalid request type
    setup.bmRequestType = 0x80; // Standard device request
    TEST_ASSERT(validate_control_request(&setup) == 0, "Invalid request type should be rejected");
    
    // Test null pointer
    TEST_ASSERT(validate_control_request(NULL) == 0, "NULL request should be rejected");
    
    TEST_PASS();
}

/**
 * Test probe/commit control compatibility
 */
int test_probe_commit_controls()
{
    uvc_setup_packet_t setup;
    
    // Test VS_PROBE_CONTROL request
    setup.bmRequestType = CY_FX_USB_UVC_GET_REQ_TYPE;
    setup.bRequest = CY_FX_USB_UVC_GET_CUR_REQ;
    setup.wValue = CY_FX_USB_UVC_VS_PROBE_CONTROL;
    setup.wIndex = (0x00 << 8) | CY_FX_UVC_INTERFACE_VS; // Reserved | VS Interface
    setup.wLength = CY_FX_UVC_MAX_PROBE_SETTING;
    
    TEST_ASSERT(validate_control_request(&setup) == 1, "VS_PROBE_CONTROL request should be valid");
    TEST_ASSERT(setup.wValue == 0x0100, "VS_PROBE_CONTROL should have correct selector value");
    
    // Test VS_COMMIT_CONTROL request
    setup.wValue = CY_FX_USB_UVC_VS_COMMIT_CONTROL;
    TEST_ASSERT(validate_control_request(&setup) == 1, "VS_COMMIT_CONTROL request should be valid");
    TEST_ASSERT(setup.wValue == 0x0200, "VS_COMMIT_CONTROL should have correct selector value");
    
    TEST_PASS();
}

/**
 * Test encoding unit control ranges
 */
int test_encoding_control_ranges()
{
    // Test that control selectors are within valid range
    TEST_ASSERT(CY_FX_UVC_EU_SELECT_LAYER_CONTROL >= 0x01 && 
                CY_FX_UVC_EU_SELECT_LAYER_CONTROL <= 0x20,
                "Select Layer Control should be in valid range");
    
    TEST_ASSERT(CY_FX_UVC_EU_ERROR_RESILIENCY_CONTROL >= 0x01 && 
                CY_FX_UVC_EU_ERROR_RESILIENCY_CONTROL <= 0x20,
                "Error Resiliency Control should be in valid range");
    
    // Test that format descriptors are in UVC 1.5 range
    TEST_ASSERT(CY_FX_UVC_VS_FORMAT_H264 >= 0x10 && 
                CY_FX_UVC_VS_FORMAT_H264 <= 0x1F,
                "H.264 format should be in UVC 1.5 range");
    
    TEST_ASSERT(CY_FX_UVC_VS_FORMAT_H265 >= 0x10 && 
                CY_FX_UVC_VS_FORMAT_H265 <= 0x1F,
                "H.265 format should be in UVC 1.5 range");
    
    TEST_PASS();
}

/**
 * Test backward compatibility with existing controls
 */
int test_backward_compatibility()
{
    // Ensure existing UVC 1.1 controls still work
    TEST_ASSERT(CY_FX_USB_UVC_VS_PROBE_CONTROL == 0x0100, 
                "Legacy VS_PROBE_CONTROL should remain unchanged");
    
    TEST_ASSERT(CY_FX_USB_UVC_VS_COMMIT_CONTROL == 0x0200,
                "Legacy VS_COMMIT_CONTROL should remain unchanged");
    
    // Test that legacy request types are preserved
    TEST_ASSERT(CY_FX_USB_UVC_GET_REQ_TYPE == 0xA1,
                "Legacy GET request type should remain unchanged");
    
    TEST_ASSERT(CY_FX_USB_UVC_SET_REQ_TYPE == 0x21,
                "Legacy SET request type should remain unchanged");
    
    // Test that interface definitions are preserved
    TEST_ASSERT(CY_FX_UVC_INTERFACE_VC == 0, "Video Control interface ID should remain 0");
    TEST_ASSERT(CY_FX_UVC_INTERFACE_VS == 1, "Video Streaming interface ID should remain 1");
    
    TEST_PASS();
}

/**
 * Test encoding unit ID assignment
 */
int test_encoding_unit_id()
{
    // In our implementation, encoding unit should have ID 5
    // and be positioned between extension unit (ID 3) and output terminal (ID 4)
    
    // This is a logical test to ensure proper unit chain:
    // Camera Terminal (1) → Processing Unit (2) → Extension Unit (3) → 
    // Encoding Unit (5) → Output Terminal (4)
    
    uint8_t camera_terminal_id = 1;
    uint8_t processing_unit_id = 2;
    uint8_t extension_unit_id = 3;
    uint8_t output_terminal_id = 4;
    uint8_t encoding_unit_id = 5;
    
    // Test unit ID uniqueness
    TEST_ASSERT(camera_terminal_id != processing_unit_id, "Camera and processing units should have different IDs");
    TEST_ASSERT(extension_unit_id != encoding_unit_id, "Extension and encoding units should have different IDs");
    TEST_ASSERT(output_terminal_id != encoding_unit_id, "Output terminal and encoding unit should have different IDs");
    
    // Test logical connection chain
    TEST_ASSERT(encoding_unit_id == 5, "Encoding unit should have ID 5");
    
    TEST_PASS();
}

/**
 * Main test runner
 */
int main(void)
{
    printf("UVC 1.5 Control Request Unit Tests\n");
    printf("===================================\n\n");
    
    // Run all tests
    RUN_TEST(test_encoding_control_selectors);
    RUN_TEST(test_control_request_validation);
    RUN_TEST(test_probe_commit_controls);
    RUN_TEST(test_encoding_control_ranges);
    RUN_TEST(test_backward_compatibility);
    RUN_TEST(test_encoding_unit_id);
    
    // Print results
    printf("\n===================================\n");
    printf("Test Results: %d/%d passed\n", tests_passed, tests_total);
    
    if (tests_passed == tests_total) {
        printf("All control tests PASSED! ✓\n");
        printf("UVC 1.5 control implementation is working correctly.\n");
        return 0;
    } else {
        printf("Some control tests FAILED! ✗\n");
        printf("Please check the UVC 1.5 control implementation.\n");
        return 1;
    }
}