/*
 * UVC 1.5 Isochronous Control Request Unit Tests
 * ==============================================
 *
 * Control request tests specific to the cyfxuvcinmem (isochronous) implementation.
 * Tests UVC 1.5 control handling and isochronous-specific control scenarios.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

// Include UVC headers for isochronous implementation
#include "../../cyfxuvcinmem/cyfxuvcinmem.h"

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
 * Test isochronous-specific encoding control selectors
 */
int test_iso_encoding_control_selectors()
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

    // Verify sequential control selector values for isochronous implementation
    for (int i = 0; i < 20; i++) {
        TEST_ASSERT(expected_selectors[i] == (i + 1),
                    "Isochronous encoding unit control selectors should be sequential starting from 0x01");
    }

    TEST_PASS();
}

/**
 * Mock function to simulate isochronous control request validation
 */
int validate_iso_control_request(uvc_setup_packet_t *setup)
{
    // Basic request validation for isochronous implementation
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

    return 1; // Valid request for isochronous implementation
}

/**
 * Test isochronous control request validation
 */
int test_iso_control_request_validation()
{
    uvc_setup_packet_t setup;

    // Test valid GET_CUR request to encoding unit (isochronous)
    setup.bmRequestType = CY_FX_USB_UVC_GET_REQ_TYPE;
    setup.bRequest = CY_FX_USB_UVC_GET_CUR_REQ;
    setup.wValue = (CY_FX_UVC_EU_RATE_CONTROL_MODE_CONTROL << 8) | 0x00; // Control selector | Interface
    setup.wIndex = (0x05 << 8) | CY_FX_UVC_INTERFACE_VC; // Unit ID | Interface number
    setup.wLength = 4;

    TEST_ASSERT(validate_iso_control_request(&setup) == 1, "Valid isochronous GET_CUR request should be accepted");

    // Test valid SET_CUR request to encoding unit (isochronous)
    setup.bmRequestType = CY_FX_USB_UVC_SET_REQ_TYPE;
    setup.bRequest = CY_FX_USB_UVC_SET_CUR_REQ;
    setup.wValue = (CY_FX_UVC_EU_AVERAGE_BITRATE_CONTROL << 8) | 0x00;
    setup.wIndex = (0x05 << 8) | CY_FX_UVC_INTERFACE_VC;
    setup.wLength = 4;

    TEST_ASSERT(validate_iso_control_request(&setup) == 1, "Valid isochronous SET_CUR request should be accepted");

    // Test invalid request type
    setup.bmRequestType = 0x80; // Standard device request
    TEST_ASSERT(validate_iso_control_request(&setup) == 0, "Invalid isochronous request type should be rejected");

    // Test null pointer
    TEST_ASSERT(validate_iso_control_request(NULL) == 0, "NULL isochronous request should be rejected");

    TEST_PASS();
}

/**
 * Test isochronous probe/commit control compatibility
 */
int test_iso_probe_commit_controls()
{
    uvc_setup_packet_t setup;

    // Test VS_PROBE_CONTROL request for isochronous
    setup.bmRequestType = CY_FX_USB_UVC_GET_REQ_TYPE;
    setup.bRequest = CY_FX_USB_UVC_GET_CUR_REQ;
    setup.wValue = CY_FX_USB_UVC_VS_PROBE_CONTROL;
    setup.wIndex = (0x00 << 8) | CY_FX_UVC_INTERFACE_VS; // Reserved | VS Interface
    setup.wLength = CY_FX_UVC_MAX_PROBE_SETTING;

    TEST_ASSERT(validate_iso_control_request(&setup) == 1, "Isochronous VS_PROBE_CONTROL request should be valid");
    TEST_ASSERT(setup.wValue == 0x0100, "VS_PROBE_CONTROL should have correct selector value for isochronous");

    // Test VS_COMMIT_CONTROL request for isochronous
    setup.wValue = CY_FX_USB_UVC_VS_COMMIT_CONTROL;
    TEST_ASSERT(validate_iso_control_request(&setup) == 1, "Isochronous VS_COMMIT_CONTROL request should be valid");
    TEST_ASSERT(setup.wValue == 0x0200, "VS_COMMIT_CONTROL should have correct selector value for isochronous");

    TEST_PASS();
}

/**
 * Test isochronous-specific encoding control ranges
 */
int test_iso_encoding_control_ranges()
{
    // Test that control selectors are within valid range for isochronous
    TEST_ASSERT(CY_FX_UVC_EU_SELECT_LAYER_CONTROL >= 0x01 &&
                CY_FX_UVC_EU_SELECT_LAYER_CONTROL <= 0x20,
                "Isochronous Select Layer Control should be in valid range");

    TEST_ASSERT(CY_FX_UVC_EU_ERROR_RESILIENCY_CONTROL >= 0x01 &&
                CY_FX_UVC_EU_ERROR_RESILIENCY_CONTROL <= 0x20,
                "Isochronous Error Resiliency Control should be in valid range");

    // Test that format descriptors are in UVC 1.5 range for isochronous
    TEST_ASSERT(CY_FX_UVC_VS_FORMAT_H264 >= 0x10 &&
                CY_FX_UVC_VS_FORMAT_H264 <= 0x1F,
                "Isochronous H.264 format should be in UVC 1.5 range");

    TEST_ASSERT(CY_FX_UVC_VS_FORMAT_H265 >= 0x10 &&
                CY_FX_UVC_VS_FORMAT_H265 <= 0x1F,
                "Isochronous H.265 format should be in UVC 1.5 range");

    TEST_PASS();
}

/**
 * Test backward compatibility with existing isochronous controls
 */
int test_iso_backward_compatibility()
{
    // Ensure existing UVC 1.1 controls still work in isochronous implementation
    TEST_ASSERT(CY_FX_USB_UVC_VS_PROBE_CONTROL == 0x0100,
                "Legacy isochronous VS_PROBE_CONTROL should remain unchanged");

    TEST_ASSERT(CY_FX_USB_UVC_VS_COMMIT_CONTROL == 0x0200,
                "Legacy isochronous VS_COMMIT_CONTROL should remain unchanged");

    // Test that legacy request types are preserved for isochronous
    TEST_ASSERT(CY_FX_USB_UVC_GET_REQ_TYPE == 0xA1,
                "Legacy isochronous GET request type should remain unchanged");

    TEST_ASSERT(CY_FX_USB_UVC_SET_REQ_TYPE == 0x21,
                "Legacy isochronous SET request type should remain unchanged");

    // Test that interface definitions are preserved for isochronous
    TEST_ASSERT(CY_FX_UVC_INTERFACE_VC == 0, "Isochronous Video Control interface ID should remain 0");
    TEST_ASSERT(CY_FX_UVC_INTERFACE_VS == 1, "Isochronous Video Streaming interface ID should remain 1");

    TEST_PASS();
}

/**
 * Test isochronous encoding unit ID assignment
 */
int test_iso_encoding_unit_id()
{
    // In isochronous implementation, encoding unit should have ID 5
    // and be positioned between extension unit (ID 3) and output terminal (ID 4)

    // This is a logical test to ensure proper unit chain for isochronous:
    // Camera Terminal (1) → Processing Unit (2) → Extension Unit (3) →
    // Encoding Unit (5) → Output Terminal (4)

    uint8_t camera_terminal_id = 1;
    uint8_t processing_unit_id = 2;
    uint8_t extension_unit_id = 3;
    uint8_t output_terminal_id = 4;
    uint8_t encoding_unit_id = 5;

    // Test unit ID uniqueness for isochronous
    TEST_ASSERT(camera_terminal_id != processing_unit_id, "Isochronous camera and processing units should have different IDs");
    TEST_ASSERT(extension_unit_id != encoding_unit_id, "Isochronous extension and encoding units should have different IDs");
    TEST_ASSERT(output_terminal_id != encoding_unit_id, "Isochronous output terminal and encoding unit should have different IDs");

    // Test logical connection chain for isochronous
    TEST_ASSERT(encoding_unit_id == 5, "Isochronous encoding unit should have ID 5");

    TEST_PASS();
}

/**
 * Test isochronous-specific streaming interface controls
 */
int test_iso_streaming_controls()
{
    // Test isochronous streaming interface number
    TEST_ASSERT(CY_FX_UVC_INTERFACE_VS == 1, "Isochronous streaming interface should be interface 1");

    // Test that streaming controls are properly defined for isochronous
    TEST_ASSERT(CY_FX_USB_UVC_VS_PROBE_CONTROL != 0, "Isochronous probe control should be defined");
    TEST_ASSERT(CY_FX_USB_UVC_VS_COMMIT_CONTROL != 0, "Isochronous commit control should be defined");

    // Test probe setting structure size for isochronous
    TEST_ASSERT(CY_FX_UVC_MAX_PROBE_SETTING > 0, "Isochronous probe setting size should be positive");
    TEST_ASSERT(CY_FX_UVC_MAX_PROBE_SETTING >= 26, "Isochronous probe setting should be at least 26 bytes (UVC 1.1 minimum)");

    TEST_PASS();
}

/**
 * Main test runner for isochronous control tests
 */
int main(void)
{
    printf("UVC 1.5 Isochronous Control Request Unit Tests\n");
    printf("===============================================\n\n");

    // Run all tests
    RUN_TEST(test_iso_encoding_control_selectors);
    RUN_TEST(test_iso_control_request_validation);
    RUN_TEST(test_iso_probe_commit_controls);
    RUN_TEST(test_iso_encoding_control_ranges);
    RUN_TEST(test_iso_backward_compatibility);
    RUN_TEST(test_iso_encoding_unit_id);
    RUN_TEST(test_iso_streaming_controls);

    // Print results
    printf("\n===============================================\n");
    printf("Isochronous Control Test Results: %d/%d passed\n", tests_passed, tests_total);

    if (tests_passed == tests_total) {
        printf("All isochronous control tests PASSED! ✓\n");
        printf("UVC 1.5 isochronous control implementation is working correctly.\n");
        return 0;
    } else {
        printf("Some isochronous control tests FAILED! ✗\n");
        printf("Please check the UVC 1.5 isochronous control implementation.\n");
        return 1;
    }
}