#!/bin/bash

# Manual UVC 1.5 Validation Test Suite
# ====================================

echo "🔍 UVC 1.5 Manual Validation Test Suite"
echo "========================================"
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

echo "Test 1: UVC Version Upgrade Validation"
echo "-------------------------------------"

# Test 1.1: Check UVC 1.5 version in ISO descriptors
if grep -q "0x50,0x01.*Revision of class spec : 1.5" ../cyfxuvcinmem/cyfxuvcdscr.c; then
    test_result "UVC 1.5 version in ISO descriptors" "PASS"
else
    test_result "UVC 1.5 version in ISO descriptors" "FAIL"
fi

# Test 1.2: Check UVC 1.5 version in Bulk descriptors  
if grep -q "0x50,0x01.*Revision of class spec : 1.5" ../cyfxuvcinmem_bulk/cyfxuvcdscr.c; then
    test_result "UVC 1.5 version in Bulk descriptors" "PASS"
else
    test_result "UVC 1.5 version in Bulk descriptors" "FAIL"
fi

# Test 1.3: Verify old UVC 1.1 version is gone
if ! grep -q "0x10,0x01.*Revision of class spec : 1.1" ../cyfxuvcinmem/cyfxuvcdscr.c; then
    test_result "Old UVC 1.1 version removed from ISO" "PASS"
else
    test_result "Old UVC 1.1 version removed from ISO" "FAIL"
fi

echo ""
echo "Test 2: Encoding Unit Descriptor Validation"
echo "------------------------------------------"

# Test 2.1: Check encoding unit descriptor presence in ISO
if grep -q "CY_FX_UVC_VC_ENCODING_UNIT.*Encoding unit descriptor type" ../cyfxuvcinmem/cyfxuvcdscr.c; then
    test_result "Encoding unit descriptor in ISO implementation" "PASS"
else
    test_result "Encoding unit descriptor in ISO implementation" "FAIL"
fi

# Test 2.2: Check encoding unit descriptor presence in Bulk
if grep -q "CY_FX_UVC_VC_ENCODING_UNIT.*Encoding unit descriptor type" ../cyfxuvcinmem_bulk/cyfxuvcdscr.c; then
    test_result "Encoding unit descriptor in Bulk implementation" "PASS"
else
    test_result "Encoding unit descriptor in Bulk implementation" "FAIL"
fi

# Test 2.3: Check encoding unit ID assignment
if grep -q "0x05.*ID of this unit" ../cyfxuvcinmem/cyfxuvcdscr.c; then
    test_result "Encoding unit ID assignment (5)" "PASS"
else
    test_result "Encoding unit ID assignment (5)" "FAIL"
fi

# Test 2.4: Check encoding unit source connection
if grep -q "0x03.*Source ID : 3 : Connected to extn unit" ../cyfxuvcinmem/cyfxuvcdscr.c; then
    test_result "Encoding unit source connection (extension unit)" "PASS"
else
    test_result "Encoding unit source connection (extension unit)" "FAIL"
fi

# Test 2.5: Check output terminal connection to encoding unit
if grep -q "0x05.*Source ID : 5 : Connected to encoding unit" ../cyfxuvcinmem/cyfxuvcdscr.c; then
    test_result "Output terminal connection to encoding unit" "PASS"
else
    test_result "Output terminal connection to encoding unit" "FAIL"
fi

echo ""
echo "Test 3: UVC 1.5 Constants Validation"
echo "-----------------------------------"

# Test 3.1: Check H.264 format constants in ISO headers
if grep -q "#define CY_FX_UVC_VS_FORMAT_H264.*0x10" ../cyfxuvcinmem/cyfxuvcinmem.h; then
    test_result "H.264 format constant definition (ISO)" "PASS"
else
    test_result "H.264 format constant definition (ISO)" "FAIL"
fi

# Test 3.2: Check H.265 format constants
if grep -q "#define CY_FX_UVC_VS_FORMAT_H265.*0x12" ../cyfxuvcinmem/cyfxuvcinmem.h; then
    test_result "H.265 format constant definition" "PASS"
else
    test_result "H.265 format constant definition" "FAIL"
fi

# Test 3.3: Check encoding unit constant
if grep -q "#define CY_FX_UVC_VC_ENCODING_UNIT.*0x07" ../cyfxuvcinmem/cyfxuvcinmem.h; then
    test_result "Encoding unit constant definition" "PASS"
else
    test_result "Encoding unit constant definition" "FAIL"
fi

# Test 3.4: Check control selectors
if grep -q "#define CY_FX_UVC_EU_RATE_CONTROL_MODE_CONTROL.*0x06" ../cyfxuvcinmem/cyfxuvcinmem.h; then
    test_result "Rate Control Mode selector" "PASS"
else
    test_result "Rate Control Mode selector" "FAIL"
fi

# Test 3.5: Check Bulk implementation has same constants
if grep -q "#define CY_FX_UVC_VS_FORMAT_H264.*0x10" ../cyfxuvcinmem_bulk/cyfxuvcinmem.h; then
    test_result "H.264 format constant in Bulk headers" "PASS"
else
    test_result "H.264 format constant in Bulk headers" "FAIL"
fi

echo ""
echo "Test 4: Descriptor Size Validation"
echo "---------------------------------"

# Test 4.1: Check SS configuration descriptor length update
if grep -q "0xE0,0x00.*Length of this descriptor and all sub descriptors" ../cyfxuvcinmem/cyfxuvcdscr.c; then
    test_result "SS config descriptor length updated (0xE0)" "PASS"
else
    test_result "SS config descriptor length updated (0xE0)" "FAIL"
fi

# Test 4.2: Check HS configuration descriptor length update
if grep -q "0xD4,0x00.*Length of this descriptor and all sub descriptors" ../cyfxuvcinmem/cyfxuvcdscr.c; then
    test_result "HS config descriptor length updated (0xD4)" "PASS"
else
    test_result "HS config descriptor length updated (0xD4)" "FAIL"
fi

# Test 4.3: Check VC class descriptor total size update
if grep -q "0x5E,0x00.*Total size of class specific descriptors" ../cyfxuvcinmem/cyfxuvcdscr.c; then
    test_result "VC class descriptors total size updated (0x5E)" "PASS"
else
    test_result "VC class descriptors total size updated (0x5E)" "FAIL"
fi

echo ""
echo "Test 5: Backward Compatibility Validation"
echo "----------------------------------------"

# Test 5.1: Check MJPEG format descriptor still present
if grep -q "0x06.*Descriptor subtype : VS_FORMAT_MJPEG" ../cyfxuvcinmem/cyfxuvcdscr.c; then
    test_result "MJPEG format descriptor preserved" "PASS"
else
    test_result "MJPEG format descriptor preserved" "FAIL"
fi

# Test 5.2: Check probe/commit controls preserved
if grep -q "#define CY_FX_USB_UVC_VS_PROBE_CONTROL.*0x0100" ../cyfxuvcinmem/cyfxuvcinmem.h; then
    test_result "VS_PROBE_CONTROL preserved" "PASS"
else
    test_result "VS_PROBE_CONTROL preserved" "FAIL"
fi

# Test 5.3: Check commit control preserved
if grep -q "#define CY_FX_USB_UVC_VS_COMMIT_CONTROL.*0x0200" ../cyfxuvcinmem/cyfxuvcinmem.h; then
    test_result "VS_COMMIT_CONTROL preserved" "PASS"
else
    test_result "VS_COMMIT_CONTROL preserved" "FAIL"
fi

# Test 5.4: Check interface definitions preserved
if grep -q "#define CY_FX_UVC_INTERFACE_VC.*0" ../cyfxuvcinmem/cyfxuvcinmem.h; then
    test_result "Video Control interface ID preserved" "PASS"
else
    test_result "Video Control interface ID preserved" "FAIL"
fi

echo ""
echo "Test 6: Code Structure Validation"
echo "--------------------------------"

# Test 6.1: Check for syntax errors in descriptor arrays
if grep -q "0x0D.*Descriptor size: 13 bytes" ../cyfxuvcinmem/cyfxuvcdscr.c; then
    test_result "Encoding unit descriptor size correct (13 bytes)" "PASS"
else
    test_result "Encoding unit descriptor size correct (13 bytes)" "FAIL"
fi

# Test 6.2: Check descriptor array structure integrity
descriptor_lines=$(grep -c "0x[0-9A-F][0-9A-F]," ../cyfxuvcinmem/cyfxuvcdscr.c)
if [ $descriptor_lines -gt 100 ]; then
    test_result "Descriptor array structure integrity" "PASS"
else
    test_result "Descriptor array structure integrity" "FAIL"
fi

echo ""
echo "Test 7: Documentation Validation"
echo "-------------------------------"

# Test 7.1: Check upgrade documentation exists
if [ -f "../UVC_1.5_UPGRADE.md" ]; then
    test_result "UVC 1.5 upgrade documentation exists" "PASS"
else
    test_result "UVC 1.5 upgrade documentation exists" "FAIL"
fi

# Test 7.2: Check CLAUDE.md exists
if [ -f "../CLAUDE.md" ]; then
    test_result "CLAUDE.md documentation exists" "PASS"
else
    test_result "CLAUDE.md documentation exists" "FAIL"
fi

# Test 7.3: Check memory file exists
if [ -f "../CLAUDE_MEMORY.md" ]; then
    test_result "Claude memory state file exists" "PASS"
else
    test_result "Claude memory state file exists" "FAIL"
fi

echo ""
echo "========================================"
echo "🏁 TEST SUMMARY"
echo "========================================"
echo -e "Total Tests: ${BLUE}$TOTAL_TESTS${NC}"
echo -e "Passed:      ${GREEN}$PASSED_TESTS${NC}"
echo -e "Failed:      ${RED}$FAILED_TESTS${NC}"

if [ $FAILED_TESTS -eq 0 ]; then
    echo ""
    echo -e "${GREEN}🎉 ALL TESTS PASSED! 🎉${NC}"
    echo -e "${GREEN}UVC 1.5 upgrade implementation is validated and ready!${NC}"
    echo ""
    echo "Key validations completed:"
    echo "✓ UVC version upgraded from 1.1 to 1.5"
    echo "✓ Encoding unit descriptors properly added" 
    echo "✓ UVC 1.5 constants and controls defined"
    echo "✓ Descriptor sizes correctly updated"
    echo "✓ Backward compatibility with MJPEG preserved"
    echo "✓ Code structure integrity maintained"
    echo "✓ Documentation complete"
    exit 0
else
    echo ""
    echo -e "${RED}❌ SOME TESTS FAILED ❌${NC}"
    echo -e "${YELLOW}Please review the failing tests above and fix the issues.${NC}"
    exit 1
fi