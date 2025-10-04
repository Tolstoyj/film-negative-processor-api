#!/bin/bash

# Film Processor API Test Script

API_URL="http://localhost:8080"
TEST_IMAGE="$1"

if [ -z "$TEST_IMAGE" ]; then
    echo "Usage: $0 <test_image.jpg>"
    exit 1
fi

if [ ! -f "$TEST_IMAGE" ]; then
    echo "Error: Image file '$TEST_IMAGE' not found"
    exit 1
fi

echo "=== Film Processor API Test ==="
echo ""

# Test 1: Health Check
echo "1. Testing health endpoint..."
curl -s "$API_URL/health" | python -m json.tool
echo ""

# Test 2: API Info
echo "2. Testing API info endpoint..."
curl -s "$API_URL/" | python -m json.tool
echo ""

# Test 3: Convert to Negative
echo "3. Converting image to negative..."
curl -X POST "$API_URL/api/to-negative" \
  -F "image=@$TEST_IMAGE" \
  -o test_negative.jpg \
  -w "HTTP Status: %{http_code}\n"
echo "Output saved to: test_negative.jpg"
echo ""

# Test 4: Convert back to Positive
echo "4. Converting negative back to positive..."
curl -X POST "$API_URL/api/to-positive" \
  -F "image=@test_negative.jpg" \
  -o test_restored.jpg \
  -w "HTTP Status: %{http_code}\n"
echo "Output saved to: test_restored.jpg"
echo ""

echo "=== Test Complete ==="
echo ""
echo "Results:"
echo "  - Original: $TEST_IMAGE"
echo "  - Negative: test_negative.jpg"
echo "  - Restored: test_restored.jpg"
