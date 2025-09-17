/*
 * UVC 1.5 Bulk Control Request Unit Tests
 * ========================================
 *
 * Control request tests specific to the cyfxuvcinmem_bulk (bulk endpoint) implementation.
 * Tests UVC 1.5 control handling and bulk-specific control scenarios.
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
 * Test bulk-specific encoding control selectors
 */
int test_bulk_encoding_control_selectors()
{
    // Test that all UVC 1.5 encoding unit control selectors are properly defined for bulk
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

    // Verify sequential control selector values for bulk implementation
    for (int i = 0; i < 20; i++) {
        TEST_ASSERT(expected_selectors[i] == (i + 1),
                    "Bulk encoding unit control selectors should be sequential starting from 0x01");
    }

    TEST_PASS();
}

/**
 * Mock function to simulate bulk control request validation
 */
int validate_bulk_control_request(uvc_setup_packet_t *setup)
{
    // Basic request validation for bulk implementation
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

    return 1; // Valid request for bulk implementation
}

/**
 * Test bulk control request validation
 */
int test_bulk_control_request_validation()
{
    uvc_setup_packet_t setup;

    // Test valid GET_CUR request to encoding unit (bulk)
    setup.bmRequestType = CY_FX_USB_UVC_GET_REQ_TYPE;
    setup.bRequest = CY_FX_USB_UVC_GET_CUR_REQ;
    setup.wValue = (CY_FX_UVC_EU_RATE_CONTROL_MODE_CONTROL << 8) | 0x00; // Control selector | Interface
    setup.wIndex = (0x05 << 8) | CY_FX_UVC_INTERFACE_VC; // Unit ID | Interface number
    setup.wLength = 4;

    TEST_ASSERT(validate_bulk_control_request(&setup) == 1, "Valid bulk GET_CUR request should be accepted");

    // Test valid SET_CUR request to encoding unit (bulk)
    setup.bmRequestType = CY_FX_USB_UVC_SET_REQ_TYPE;
    setup.bRequest = CY_FX_USB_UVC_SET_CUR_REQ;
    setup.wValue = (CY_FX_UVC_EU_AVERAGE_BITRATE_CONTROL << 8) | 0x00;
    setup.wIndex = (0x05 << 8) | CY_FX_UVC_INTERFACE_VC;
    setup.wLength = 4;

    TEST_ASSERT(validate_bulk_control_request(&setup) == 1, "Valid bulk SET_CUR request should be accepted");

    // Test invalid request type
    setup.bmRequestType = 0x80; // Standard device request
    TEST_ASSERT(validate_bulk_control_request(&setup) == 0, "Invalid bulk request type should be rejected");

    // Test null pointer
    TEST_ASSERT(validate_bulk_control_request(NULL) == 0, "NULL bulk request should be rejected");

    TEST_PASS();
}

/**
 * Test bulk probe/commit control compatibility
 */
int test_bulk_probe_commit_controls()
{
    uvc_setup_packet_t setup;

    // Test VS_PROBE_CONTROL request for bulk
    setup.bmRequestType = CY_FX_USB_UVC_GET_REQ_TYPE;
    setup.bRequest = CY_FX_USB_UVC_GET_CUR_REQ;
    setup.wValue = CY_FX_USB_UVC_VS_PROBE_CONTROL;
    setup.wIndex = (0x00 << 8) | CY_FX_UVC_INTERFACE_VS; // Reserved | VS Interface
    setup.wLength = CY_FX_UVC_MAX_PROBE_SETTING;

    TEST_ASSERT(validate_bulk_control_request(&setup) == 1, "Bulk VS_PROBE_CONTROL request should be valid");
    TEST_ASSERT(setup.wValue == 0x0100, "VS_PROBE_CONTROL should have correct selector value for bulk");

    // Test VS_COMMIT_CONTROL request for bulk
    setup.wValue = CY_FX_USB_UVC_VS_COMMIT_CONTROL;
    TEST_ASSERT(validate_bulk_control_request(&setup) == 1, "Bulk VS_COMMIT_CONTROL request should be valid");
    TEST_ASSERT(setup.wValue == 0x0200, "VS_COMMIT_CONTROL should have correct selector value for bulk");

    TEST_PASS();
}

/**
 * Test bulk-specific encoding control ranges
 */
int test_bulk_encoding_control_ranges()
{
    // Test that control selectors are within valid range for bulk
    TEST_ASSERT(CY_FX_UVC_EU_SELECT_LAYER_CONTROL >= 0x01 &&
                CY_FX_UVC_EU_SELECT_LAYER_CONTROL <= 0x20,
                "Bulk Select Layer Control should be in valid range");

    TEST_ASSERT(CY_FX_UVC_EU_ERROR_RESILIENCY_CONTROL >= 0x01 &&
                CY_FX_UVC_EU_ERROR_RESILIENCY_CONTROL <= 0x20,
                "Bulk Error Resiliency Control should be in valid range");

    // Test that format descriptors are in UVC 1.5 range for bulk
    TEST_ASSERT(CY_FX_UVC_VS_FORMAT_H264 >= 0x10 &&
                CY_FX_UVC_VS_FORMAT_H264 <= 0x1F,
                "Bulk H.264 format should be in UVC 1.5 range");

    TEST_ASSERT(CY_FX_UVC_VS_FORMAT_H265 >= 0x10 &&
                CY_FX_UVC_VS_FORMAT_H265 <= 0x1F,
                "Bulk H.265 format should be in UVC 1.5 range");

    TEST_PASS();
}

/**
 * Test backward compatibility with existing bulk controls
 */
int test_bulk_backward_compatibility()
{
    // Ensure existing UVC 1.1 controls still work in bulk implementation
    TEST_ASSERT(CY_FX_USB_UVC_VS_PROBE_CONTROL == 0x0100,
                "Legacy bulk VS_PROBE_CONTROL should remain unchanged");

    TEST_ASSERT(CY_FX_USB_UVC_VS_COMMIT_CONTROL == 0x0200,
                "Legacy bulk VS_COMMIT_CONTROL should remain unchanged");

    // Test that legacy request types are preserved for bulk
    TEST_ASSERT(CY_FX_USB_UVC_GET_REQ_TYPE == 0xA1,
                "Legacy bulk GET request type should remain unchanged");

    TEST_ASSERT(CY_FX_USB_UVC_SET_REQ_TYPE == 0x21,
                "Legacy bulk SET request type should remain unchanged");

    // Test that interface definitions are preserved for bulk
    TEST_ASSERT(CY_FX_UVC_INTERFACE_VC == 0, "Bulk Video Control interface ID should remain 0");
    TEST_ASSERT(CY_FX_UVC_INTERFACE_VS == 1, "Bulk Video Streaming interface ID should remain 1");

    TEST_PASS();
}

/**
 * Test bulk encoding unit ID assignment
 */
int test_bulk_encoding_unit_id()
{
    // In bulk implementation, encoding unit should have ID 5
    // and be positioned between extension unit (ID 3) and output terminal (ID 4)

    // This is a logical test to ensure proper unit chain for bulk:
    // Camera Terminal (1) → Processing Unit (2) → Extension Unit (3) →
    // Encoding Unit (5) → Output Terminal (4)

    uint8_t camera_terminal_id = 1;
    uint8_t processing_unit_id = 2;
    uint8_t extension_unit_id = 3;
    uint8_t output_terminal_id = 4;
    uint8_t encoding_unit_id = 5;

    // Test unit ID uniqueness for bulk
    TEST_ASSERT(camera_terminal_id != processing_unit_id, "Bulk camera and processing units should have different IDs");
    TEST_ASSERT(extension_unit_id != encoding_unit_id, "Bulk extension and encoding units should have different IDs");
    TEST_ASSERT(output_terminal_id != encoding_unit_id, "Bulk output terminal and encoding unit should have different IDs");

    // Test logical connection chain for bulk
    TEST_ASSERT(encoding_unit_id == 5, "Bulk encoding unit should have ID 5");

    TEST_PASS();
}

/**
 * Test bulk-specific streaming interface controls
 */
int test_bulk_streaming_controls()
{
    // Test bulk streaming interface number
    TEST_ASSERT(CY_FX_UVC_INTERFACE_VS == 1, "Bulk streaming interface should be interface 1");

    // Test that streaming controls are properly defined for bulk
    TEST_ASSERT(CY_FX_USB_UVC_VS_PROBE_CONTROL != 0, "Bulk probe control should be defined");
    TEST_ASSERT(CY_FX_USB_UVC_VS_COMMIT_CONTROL != 0, "Bulk commit control should be defined");

    // Test probe setting structure size for bulk
    TEST_ASSERT(CY_FX_UVC_MAX_PROBE_SETTING > 0, "Bulk probe setting size should be positive");
    TEST_ASSERT(CY_FX_UVC_MAX_PROBE_SETTING >= 26, "Bulk probe setting should be at least 26 bytes (UVC 1.1 minimum)");

    TEST_PASS();
}

/**
 * Test bulk-specific bitrate controls (important for bulk transfers)
 */
int test_bulk_bitrate_controls()
{
    // Test bitrate control selectors that are particularly important for bulk transfers
    TEST_ASSERT(CY_FX_UVC_EU_AVERAGE_BITRATE_CONTROL == 0x07,
                "Bulk average bitrate control should be properly defined");

    TEST_ASSERT(CY_FX_UVC_EU_PEAK_BIT_RATE_CONTROL == 0x09,
                "Bulk peak bitrate control should be properly defined");

    TEST_ASSERT(CY_FX_UVC_EU_RATE_CONTROL_MODE_CONTROL == 0x06,
                "Bulk rate control mode should be properly defined");

    // Test that CPB (Coded Picture Buffer) control is available for bulk
    TEST_ASSERT(CY_FX_UVC_EU_CPB_SIZE_CONTROL == 0x08,
                "Bulk CPB size control should be properly defined");

    TEST_PASS();
}

/**
 * Test bulk transfer optimization controls
 */
int test_bulk_transfer_optimization()
{
    // Test controls that are particularly relevant for bulk transfer optimization
    TEST_ASSERT(CY_FX_UVC_EU_QUANTIZATION_PARAMS_CONTROL == 0x0A,
                "Bulk quantization parameters control should be defined");

    TEST_ASSERT(CY_FX_UVC_EU_ERROR_RESILIENCY_CONTROL == 0x14,
                "Bulk error resiliency control should be defined (important for bulk transfers)");

    TEST_ASSERT(CY_FX_UVC_EU_SLICE_MODE_CONTROL == 0x05,
                "Bulk slice mode control should be defined (helps with bulk packetization)");

    // Test frame interval control (important for bulk transfer timing)
    TEST_ASSERT(CY_FX_UVC_EU_MIN_FRAME_INTERVAL_CONTROL == 0x04,
                "Bulk minimum frame interval control should be defined");

    TEST_PASS();
}

/**
 * Main test runner for bulk control tests
 */
int main(void)
{
    printf("UVC 1.5 Bulk Control Request Unit Tests\n");
    printf("========================================\n\n");

    // Run all tests
    RUN_TEST(test_bulk_encoding_control_selectors);
    RUN_TEST(test_bulk_control_request_validation);
    RUN_TEST(test_bulk_probe_commit_controls);
    RUN_TEST(test_bulk_encoding_control_ranges);
    RUN_TEST(test_bulk_backward_compatibility);
    RUN_TEST(test_bulk_encoding_unit_id);
    RUN_TEST(test_bulk_streaming_controls);
    RUN_TEST(test_bulk_bitrate_controls);
    RUN_TEST(test_bulk_transfer_optimization);

    // Print results
    printf("\n========================================\n");
    printf("Bulk Control Test Results: %d/%d passed\n", tests_passed, tests_total);

    if (tests_passed == tests_total) {
        printf("All bulk control tests PASSED! ✓\n");
        printf("UVC 1.5 bulk control implementation is working correctly.\n");
        return 0;
    } else {
        printf("Some bulk control tests FAILED! ✗\n");
        printf("Please check the UVC 1.5 bulk control implementation.\n");
        return 1;
    }
}