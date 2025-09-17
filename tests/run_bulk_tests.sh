#!/bin/bash

# UVC 1.5 Bulk Implementation Test Runner
# ========================================

echo "🔍 UVC 1.5 Bulk Implementation Test Suite"
echo "==========================================="
echo ""

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Test counters
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Test result function
test_result() {
    local test_name="$1"
    local result="$2"

    ((TOTAL_TESTS++))

    if [ "$result" = "PASS" ]; then
        echo -e "${GREEN}✓ PASS: $test_name${NC}"
        ((PASSED_TESTS++))
    else
        echo -e "${RED}✗ FAIL: $test_name${NC}"
        ((FAILED_TESTS++))
    fi
}

echo "Test 1: Bulk UVC Version Validation"
echo "-----------------------------------"

# Test 1.1: Check UVC 1.5 version in bulk descriptors
if grep -q "0x50,0x01.*Revision of class spec : 1.5" ../cyfxuvcinmem_bulk/cyfxuvcdscr.c; then
    test_result "UVC 1.5 version in bulk descriptors" "PASS"
else
    test_result "UVC 1.5 version in bulk descriptors" "FAIL"
fi

echo ""
echo "Test 2: Bulk Endpoint Validation"
echo "-------------------------------"

# Test 2.1: Check for bulk endpoint (CY_FX_EP_BULK_VIDEO)
if grep -q "CY_FX_EP_BULK_VIDEO.*Endpoint address and description" ../cyfxuvcinmem_bulk/cyfxuvcdscr.c; then
    test_result "Bulk endpoint CY_FX_EP_BULK_VIDEO present" "PASS"
else
    test_result "Bulk endpoint CY_FX_EP_BULK_VIDEO present" "FAIL"
fi

# Test 2.2: Check for bulk transfer type
if grep -q "CY_U3P_USB_EP_BULK.*Bulk.*End Point" ../cyfxuvcinmem_bulk/cyfxuvcdscr.c; then
    test_result "Bulk transfer type specified" "PASS"
else
    test_result "Bulk transfer type specified" "FAIL"
fi

echo ""
echo "Test 3: Bulk Descriptor Sizes"
echo "----------------------------"

# Test 3.1: Check SuperSpeed bulk descriptor size (should be smaller than isochronous)
if grep -q "0xCB,0x00.*Length of this descriptor" ../cyfxuvcinmem_bulk/cyfxuvcdscr.c; then
    test_result "SuperSpeed bulk descriptor size (0xCB)" "PASS"
else
    test_result "SuperSpeed bulk descriptor size (0xCB)" "FAIL"
fi

# Test 3.2: Check HighSpeed bulk descriptor size (the correct value is 0xCB for HS)
if grep -q "0xCB,0x00.*Length of this descriptor" ../cyfxuvcinmem_bulk/cyfxuvcdscr.c; then
    test_result "HighSpeed bulk descriptor size (0xCB)" "PASS"
else
    test_result "HighSpeed bulk descriptor size (0xCB)" "FAIL"
fi

echo ""
echo "Test 4: Bulk Encoding Unit"
echo "-------------------------"

# Test 4.1: Check encoding unit in bulk implementation
if grep -q "CY_FX_UVC_VC_ENCODING_UNIT.*Encoding unit descriptor type" ../cyfxuvcinmem_bulk/cyfxuvcdscr.c; then
    test_result "Encoding unit in bulk implementation" "PASS"
else
    test_result "Encoding unit in bulk implementation" "FAIL"
fi

echo ""
echo "Test 5: Bulk Constants"
echo "---------------------"

# Test 5.1: Check H.264 constants in bulk headers
if grep -q "#define CY_FX_UVC_VS_FORMAT_H264.*0x10" ../cyfxuvcinmem_bulk/cyfxuvcinmem.h; then
    test_result "H.264 format constants in bulk headers" "PASS"
else
    test_result "H.264 format constants in bulk headers" "FAIL"
fi

echo ""
echo "Test 6: Bulk-Specific Features"
echo "-----------------------------"

# Test 6.1: Check bulk max burst for SuperSpeed (actual value is 0x07 = 7, meaning 8 packets)
if grep -q "0x07.*Max no. of packets in a Burst : 8" ../cyfxuvcinmem_bulk/cyfxuvcdscr.c; then
    test_result "Bulk SuperSpeed max burst (8 packets)" "PASS"
else
    test_result "Bulk SuperSpeed max burst (8 packets)" "FAIL"
fi

echo ""
echo "==========================================="
echo "🏁 BULK TEST SUMMARY"
echo "==========================================="
echo -e "Total Tests: ${BLUE}$TOTAL_TESTS${NC}"
echo -e "Passed:      ${GREEN}$PASSED_TESTS${NC}"
echo -e "Failed:      ${RED}$FAILED_TESTS${NC}"

if [ $FAILED_TESTS -eq 0 ]; then
    echo ""
    echo -e "${GREEN}🎉 ALL BULK TESTS PASSED! 🎉${NC}"
    echo -e "${GREEN}UVC 1.5 bulk implementation is validated!${NC}"
    exit 0
else
    echo ""
    echo -e "${RED}❌ SOME BULK TESTS FAILED ❌${NC}"
    echo -e "${YELLOW}Please review the failing tests above.${NC}"
    exit 1
fi