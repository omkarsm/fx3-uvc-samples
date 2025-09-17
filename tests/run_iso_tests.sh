#!/bin/bash

# UVC 1.5 Isochronous Implementation Test Runner
# ==============================================

echo "🔍 UVC 1.5 Isochronous Implementation Test Suite"
echo "================================================"
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

echo "Test 1: Isochronous UVC Version Validation"
echo "-----------------------------------------"

# Test 1.1: Check UVC 1.5 version in isochronous descriptors
if grep -q "0x50,0x01.*Revision of class spec : 1.5" ../cyfxuvcinmem/cyfxuvcdscr.c; then
    test_result "UVC 1.5 version in isochronous descriptors" "PASS"
else
    test_result "UVC 1.5 version in isochronous descriptors" "FAIL"
fi

echo ""
echo "Test 2: Isochronous Endpoint Validation"
echo "--------------------------------------"

# Test 2.1: Check for isochronous endpoint (CY_FX_EP_ISO_VIDEO)
if grep -q "CY_FX_EP_ISO_VIDEO.*Endpoint address and description" ../cyfxuvcinmem/cyfxuvcdscr.c; then
    test_result "Isochronous endpoint CY_FX_EP_ISO_VIDEO present" "PASS"
else
    test_result "Isochronous endpoint CY_FX_EP_ISO_VIDEO present" "FAIL"
fi

# Test 2.2: Check for isochronous transfer type
if grep -q "CY_U3P_USB_EP_ISO.*ISO end point" ../cyfxuvcinmem/cyfxuvcdscr.c; then
    test_result "Isochronous transfer type specified" "PASS"
else
    test_result "Isochronous transfer type specified" "FAIL"
fi

echo ""
echo "Test 3: Isochronous Descriptor Sizes"
echo "-----------------------------------"

# Test 3.1: Check SuperSpeed descriptor size (should be larger due to SS companion)
if grep -q "0xE0,0x00.*Length of this descriptor" ../cyfxuvcinmem/cyfxuvcdscr.c; then
    test_result "SuperSpeed isochronous descriptor size (0xE0)" "PASS"
else
    test_result "SuperSpeed isochronous descriptor size (0xE0)" "FAIL"
fi

echo ""
echo "Test 4: Isochronous Encoding Unit"
echo "--------------------------------"

# Test 4.1: Check encoding unit in isochronous implementation
if grep -q "CY_FX_UVC_VC_ENCODING_UNIT.*Encoding unit descriptor type" ../cyfxuvcinmem/cyfxuvcdscr.c; then
    test_result "Encoding unit in isochronous implementation" "PASS"
else
    test_result "Encoding unit in isochronous implementation" "FAIL"
fi

echo ""
echo "Test 5: Isochronous Constants"
echo "----------------------------"

# Test 5.1: Check H.264 constants in isochronous headers
if grep -q "#define CY_FX_UVC_VS_FORMAT_H264.*0x10" ../cyfxuvcinmem/cyfxuvcinmem.h; then
    test_result "H.264 format constants in isochronous headers" "PASS"
else
    test_result "H.264 format constants in isochronous headers" "FAIL"
fi

echo ""
echo "================================================"
echo "🏁 ISOCHRONOUS TEST SUMMARY"
echo "================================================"
echo -e "Total Tests: ${BLUE}$TOTAL_TESTS${NC}"
echo -e "Passed:      ${GREEN}$PASSED_TESTS${NC}"
echo -e "Failed:      ${RED}$FAILED_TESTS${NC}"

if [ $FAILED_TESTS -eq 0 ]; then
    echo ""
    echo -e "${GREEN}🎉 ALL ISOCHRONOUS TESTS PASSED! 🎉${NC}"
    echo -e "${GREEN}UVC 1.5 isochronous implementation is validated!${NC}"
    exit 0
else
    echo ""
    echo -e "${RED}❌ SOME ISOCHRONOUS TESTS FAILED ❌${NC}"
    echo -e "${YELLOW}Please review the failing tests above.${NC}"
    exit 1
fi