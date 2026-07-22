#!/bin/bash
# SIMOPEN integration stress test — loop all 4 use-case integration tests until
# TARGET simultaneous-open events have been observed, verifying every test passes.
# Each SIMOPEN must resolve in a single detection (no infinite retry loops).
#
# Usage: ./test_simopen_integration.sh [TARGET]
#   TARGET  total SIMOPEN events to accumulate before stopping (default: 25)
#
# Build requirements (must be set at compile time):
#   SHIP_NODE_DEBUG=1  (src/ship/ship_node/ship_node.c)
#     Enables SHIP-SIMOPEN prints counted by count_simopen() to track
#     simultaneous-open events across iterations.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
TARGET="${1:-25}"

TOTAL_SIMOPEN=0
TOTAL_PASS=0
TOTAL_FAIL=0
RUN=0

count_simopen() {
    local file="$1"
    local n
    n=$(grep -c "SHIP-SIMOPEN" "$file" 2>/dev/null) || n=0
    echo "$n"
}

cleanup() {
    { echo exit > /tmp/hp_pipe   2>/dev/null; } &
    { echo exit > /tmp/hems_pipe 2>/dev/null; } &
    sleep 1
    pkill -9 -f "build/hems"      2>/dev/null || true
    pkill -9 -f "build/heat_pump" 2>/dev/null || true
    sleep 1
}

printf "======================================================================\n"
printf "  SIMOPEN integration stress test  (target: %d SIMOPEN events)\n" "$TARGET"
printf "======================================================================\n"

cleanup

while [ "$TOTAL_SIMOPEN" -lt "$TARGET" ]; do
    RUN=$((RUN + 1))
    for TEST in lpc lpp mpc mgcp; do
        bash "$SCRIPT_DIR/test_${TEST}.sh" > /tmp/simopen_test_out.txt 2>&1
        RC=$?

        cleanup

        HP_SIM=$(count_simopen /tmp/hp.log)
        HEMS_SIM=$(count_simopen /tmp/hems.log)
        RUN_SIM=$((HP_SIM + HEMS_SIM))
        TOTAL_SIMOPEN=$((TOTAL_SIMOPEN + RUN_SIM))

        if [ "$RC" -eq 0 ]; then
            STATUS="PASS"
            TOTAL_PASS=$((TOTAL_PASS + 1))
        else
            STATUS="FAIL"
            TOTAL_FAIL=$((TOTAL_FAIL + 1))
            echo "  *** FAILED: run=$RUN test=$TEST — output below ***"
            cat /tmp/simopen_test_out.txt
        fi

        printf "run=%-2d test=%-4s  %s  hp_sim=%s hems_sim=%s  total_sim=%d/%d\n" \
            "$RUN" "$TEST" "$STATUS" "$HP_SIM" "$HEMS_SIM" "$TOTAL_SIMOPEN" "$TARGET"

        [ "$TOTAL_SIMOPEN" -ge "$TARGET" ] && break
    done
done

echo ""
echo "======================================================================"
echo "  SUMMARY"
echo "======================================================================"
printf "  Tests run : %d\n" "$((TOTAL_PASS + TOTAL_FAIL))"
printf "  PASS      : %d\n" "$TOTAL_PASS"
printf "  FAIL      : %d\n" "$TOTAL_FAIL"
printf "  SIMOPEN   : %d\n" "$TOTAL_SIMOPEN"
echo ""
if [ "$TOTAL_FAIL" -eq 0 ]; then
    echo "  RESULT: ALL PASSED"
else
    echo "  RESULT: $TOTAL_FAIL FAILURE(S)"
    exit 1
fi
