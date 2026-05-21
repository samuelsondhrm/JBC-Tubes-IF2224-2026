#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJ_DIR="$(dirname "$(dirname "$SCRIPT_DIR")")"
BUILD_DIR="$PROJ_DIR/build"

SEMANTIC="$BUILD_DIR/arion-semantic"
TEST_DIR="$PROJ_DIR/test/milestone-3"
OUT_DIR="$TEST_DIR/outputs"
ERR_DIR="$TEST_DIR/expected-errors"

mkdir -p "$OUT_DIR"

RED='\033[0;31m'
GREEN='\033[0;32m'
BOLD='\033[1m'
NC='\033[0m'

echo -e "${BOLD}=== Milestone 3: Semantic Analysis Tests ===${NC}"
echo ""

echo -e "${BOLD}--- Valid Programs ---${NC}"
VPASS=0; VFAIL=0
for f in "$TEST_DIR"/test-0[1-6]-*.txt; do
    [ -f "$f" ] || continue
    name=$(basename "$f" .txt)
    out="$OUT_DIR/${name}.out"

    if "$SEMANTIC" "$f" > "$out" 2>/dev/null; then
        echo -e "  ${GREEN}PASS${NC} $name"
        ((VPASS++))
    else
        echo -e "  ${RED}FAIL${NC} $name (errors detected):"
        "$SEMANTIC" "$f" 2>&1 | grep -i error | head -3
        ((VFAIL++))
    fi
done
echo -e "  ${BOLD}$VPASS passed, $VFAIL failed${NC}"
echo ""

echo -e "${BOLD}--- Error Programs ---${NC}"
EPASS=0; EFAIL=0
for f in "$TEST_DIR"/test-*err-*.txt; do
    [ -f "$f" ] || continue
    name=$(basename "$f" .txt)
    short=$(echo "$name" | sed 's/test-//')
    exp="$ERR_DIR/err-${short}.txt"

    if [ ! -f "$exp" ]; then
        echo -e "  ${RED}NO EXP${NC} $name (missing $exp)"
        ((EFAIL++))
        continue
    fi

    stderr=$("$SEMANTIC" "$f" 2>&1)
    ok=true
    while IFS= read -r pattern; do
        [ -z "$pattern" ] && continue
        if ! echo "$stderr" | grep -qF "$pattern"; then
            echo -e "  ${RED}MISS${NC} $name: expected '$pattern'"
            ok=false
            break
        fi
    done < "$exp"

    if $ok; then
        echo -e "  ${GREEN}PASS${NC} $name"
        ((EPASS++))
    else
        ((EFAIL++))
    fi
done
echo -e "  ${BOLD}$EPASS passed, $EFAIL failed${NC}"
echo ""

TOTAL=$((VPASS + VFAIL + EPASS + EFAIL))
TOTAL_PASS=$((VPASS + EPASS))
TOTAL_FAIL=$((VFAIL + EFAIL))
echo -e "${BOLD}=== Summary: $TOTAL_PASS/$TOTAL passed, $TOTAL_FAIL failed ===${NC}"
echo ""

echo -e "${BOLD}=== CTest Verification ===${NC}"
cd "$BUILD_DIR"
ctest --output-on-failure || true