#!/bin/bash
# Simultaneous-open stress test — run 20 times and verify both nodes always connect.
# Tracks which tiebreaker branch each node took per run.
#
# Build requirements (must be set at compile time):
#   SHIP_NODE_DEBUG=1  (src/ship/ship_node/ship_node.c)
#     Enables SHIP-SIMOPEN branch prints ("take server role", "yielding",
#     "server accept.*no simul") used by branch_of() to classify each run.

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
HP_LOG=/tmp/hp.log
HEMS_LOG=/tmp/hems.log
TOTAL=20
TIMEOUT=90

PASS=0
FAIL=0
TIMES=()
HP_BRANCHES=()
HEMS_BRANCHES=()

cleanup() {
    # Only write exit if the process is alive — avoids blocking opens on dead FIFOs
    if [ -f /tmp/hp.pid ]   && kill -0 "$(cat /tmp/hp.pid)"   2>/dev/null; then
        echo exit > /tmp/hp_pipe   2>/dev/null || true
    fi
    if [ -f /tmp/hems.pid ] && kill -0 "$(cat /tmp/hems.pid)" 2>/dev/null; then
        echo exit > /tmp/hems_pipe 2>/dev/null || true
    fi
    sleep 1
    { [ -f /tmp/hp.pid ]          && kill "$(cat /tmp/hp.pid)"          2>/dev/null; } || true
    { [ -f /tmp/hems.pid ]        && kill "$(cat /tmp/hems.pid)"        2>/dev/null; } || true
    { [ -f /tmp/hp_keeper.pid ]   && kill "$(cat /tmp/hp_keeper.pid)"   2>/dev/null; } || true
    { [ -f /tmp/hems_keeper.pid ] && kill "$(cat /tmp/hems_keeper.pid)" 2>/dev/null; } || true
    rm -f /tmp/hp.pid /tmp/hems.pid /tmp/hp_keeper.pid /tmp/hems_keeper.pid
    sleep 1
}

branch_of() {
    local log=$1
    if   grep -q "SHIP-SIMOPEN.*take server"      "$log" 2>/dev/null; then echo "took-server"
    elif grep -q "SHIP-SIMOPEN.*yield"            "$log" 2>/dev/null; then echo "yielded"
    elif grep -q "SHIP.*server accept.*no simul"  "$log" 2>/dev/null; then echo "no-simopen"
    elif grep -q "\[SHIP\] client connecting"     "$log" 2>/dev/null; then echo "client-only"
    else echo "unknown"
    fi
}

echo "======================================================================"
echo "  Simultaneous-open stress test  ($TOTAL runs, timeout ${TIMEOUT}s)"
echo "======================================================================"

for i in $(seq 1 $TOTAL); do
    cleanup

    "$SCRIPT_DIR/hp_pipe.sh"   > /dev/null
    "$SCRIPT_DIR/hems_pipe.sh" > /dev/null

    START=$SECONDS
    CONNECTED=false

    for t in $(seq 1 $TIMEOUT); do
        if grep -q "Remote SKI connected" "$HP_LOG"   2>/dev/null && \
           grep -q "Remote SKI connected" "$HEMS_LOG" 2>/dev/null; then
            CONNECTED=true
            break
        fi
        sleep 1
    done

    ELAPSED=$((SECONDS - START))

    if $CONNECTED; then
        HP_B=$(branch_of "$HP_LOG")
        HEMS_B=$(branch_of "$HEMS_LOG")
        printf "Run %2d/%d: PASS %3ds | hp=%-12s hems=%-12s\n" \
               "$i" "$TOTAL" "$ELAPSED" "$HP_B" "$HEMS_B"
        PASS=$((PASS + 1))
    else
        HP_B="TIMEOUT"
        HEMS_B="TIMEOUT"
        printf "Run %2d/%d: FAIL (timeout %ds)\n" "$i" "$TOTAL" "$TIMEOUT"
        FAIL=$((FAIL + 1))
    fi

    TIMES+=("$ELAPSED")
    HP_BRANCHES+=("$HP_B")
    HEMS_BRANCHES+=("$HEMS_B")
done

cleanup

# ------------------------------------------------------------------ summary
echo ""
echo "======================================================================"
echo "  SUMMARY"
echo "======================================================================"
printf "  Passed : %d / %d\n" "$PASS" "$TOTAL"
printf "  Failed : %d / %d\n" "$FAIL" "$TOTAL"

# min/max/avg
MIN=${TIMES[0]}; MAX=${TIMES[0]}; SUM=0
for t in "${TIMES[@]}"; do
    (( t < MIN )) && MIN=$t
    (( t > MAX )) && MAX=$t
    SUM=$((SUM + t))
done
AVG=$((SUM / TOTAL))
printf "  Connect time: min=%ds  avg=%ds  max=%ds\n" "$MIN" "$AVG" "$MAX"

# branch counts
HP_SERVER=$(printf '%s\n' "${HP_BRANCHES[@]}" | grep -c "took-server" || true)
HP_YIELD=$(printf '%s\n'  "${HP_BRANCHES[@]}" | grep -c "yielded"     || true)
HP_NONE=$(printf '%s\n'   "${HP_BRANCHES[@]}" | grep -c "no-simopen"  || true)
printf "  hp   branches: took-server=%d  yielded=%d  no-simopen=%d\n" \
       "$HP_SERVER" "$HP_YIELD" "$HP_NONE"

HEMS_SERVER=$(printf '%s\n' "${HEMS_BRANCHES[@]}" | grep -c "took-server" || true)
HEMS_YIELD=$(printf '%s\n'  "${HEMS_BRANCHES[@]}" | grep -c "yielded"     || true)
HEMS_NONE=$(printf '%s\n'   "${HEMS_BRANCHES[@]}" | grep -c "no-simopen"  || true)
printf "  hems branches: took-server=%d  yielded=%d  no-simopen=%d\n" \
       "$HEMS_SERVER" "$HEMS_YIELD" "$HEMS_NONE"

echo ""
if [ "$FAIL" -eq 0 ]; then
    echo "  RESULT: ALL PASSED"
else
    echo "  RESULT: $FAIL FAILURE(S)"
    exit 1
fi
