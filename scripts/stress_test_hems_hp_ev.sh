#!/bin/bash
# stress_test_hems_hp_ev.sh
# Connect HEMS + HP + EV, exchange MPC/LPC data, exit in random order.
# Repeats STRESS_ITERATIONS (default 50) times; exits with the failure count.
#
# Usage:
#   bash scripts/stress_test_hems_hp_ev.sh
#   STRESS_ITERATIONS=10 bash scripts/stress_test_hems_hp_ev.sh
#   STRESS_ITERATIONS=100 CONNECT_TIMEOUT=60 bash scripts/stress_test_hems_hp_ev.sh

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

ITERATIONS=${STRESS_ITERATIONS:-50}
CONNECT_TIMEOUT=${CONNECT_TIMEOUT:-45}   # seconds to wait for all SHIP connections
SETTLE_SECS=3                            # seconds after connect for subscriptions to settle
FAIL_DIR=/tmp/eebus_stress_fails

# OS / toolchain setup
case "$(uname -s)" in
  MINGW*|CYGWIN*|MSYS*) EXE=".exe" ;;
  *) EXE="" ;;
esac
STDBUF=()
command -v stdbuf >/dev/null 2>&1 && STDBUF=(stdbuf -oL)

# ---------------------------------------------------------------------------
# Credentials
# HEMS accepts HP (HP_SKI) as first remote and EV (EV_SKI) as --remote.
# HP/EV accept HEMS (HEMS_SKI) as their remote.
# ---------------------------------------------------------------------------
HP_SKI="${EEBUS_HP_SKI:-40c61c3526f271e8e1547851c46f6ea20d4c6f83}"
HP_CERT="${EEBUS_HP_CERT:-$SCRIPT_DIR/certificates/heat_pump.crt}"
HP_KEY="${EEBUS_HP_KEY:-$SCRIPT_DIR/certificates/heat_pump.key}"

EV_SKI="${EEBUS_EV_SKI:-5a139a8131d0b65b3078878bafd7ff71b84721a4}"
EV_CERT="${EEBUS_EV_CERT:-$SCRIPT_DIR/certificates/ev_charger.crt}"
EV_KEY="${EEBUS_EV_KEY:-$SCRIPT_DIR/certificates/ev_charger.key}"

HEMS_CERT="${EEBUS_HEMS_CERT:-$SCRIPT_DIR/certificates/hems.crt}"
HEMS_KEY="${EEBUS_HEMS_KEY:-$SCRIPT_DIR/certificates/hems.key}"
HEMS_SKI="${EEBUS_HEMS_SKI:-}"
if [ -z "$HEMS_SKI" ]; then
  HEMS_SKI=$(openssl x509 -in "$HEMS_CERT" -noout -text | \
             grep -A1 "Subject Key Identifier" | tail -1 | \
             tr -d ' :' | tr 'A-F' 'a-f')
fi

HP_PIPE=/tmp/hp_pipe
EV_PIPE=/tmp/ev_pipe
HEMS_PIPE=/tmp/hems_pipe
HP_LOG=/tmp/hp.log
EV_LOG=/tmp/ev.log
HEMS_LOG=/tmp/hems.log

mkdir -p "$FAIL_DIR"
PASS=0
FAIL=0

# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

# Fisher-Yates shuffle; prints shuffled args to stdout (bash 3.2 compatible)
shuffle_echo() {
  local arr=("$@") n=${#@} i j tmp
  for ((i=n-1; i>0; i--)); do
    j=$((RANDOM % (i+1)))
    tmp="${arr[$i]}"; arr[$i]="${arr[$j]}"; arr[$j]="$tmp"
  done
  echo "${arr[@]}"
}

kill_ports() {
  local port pids
  for port in 4710 4712 4714; do
    pids=$(lsof -ti tcp:$port 2>/dev/null || true)
    [ -n "$pids" ] && echo "$pids" | xargs kill -9 2>/dev/null || true
  done
}

cleanup_iter() {
  local pidfile
  for pidfile in /tmp/hems.pid /tmp/hp.pid /tmp/ev.pid \
                 /tmp/hems_keeper.pid /tmp/hp_keeper.pid /tmp/ev_keeper.pid; do
    [ -f "$pidfile" ] && { kill "$(cat "$pidfile")" 2>/dev/null || true; rm -f "$pidfile"; }
  done
  kill_ports
}

exit_node() {
  # Kill keeper first: closes the FIFO write end → binary gets stdin EOF → graceful exit.
  # Never write to the pipe directly — open() on a FIFO blocks if the process already died.
  case $1 in
    hems)
      [ -f /tmp/hems_keeper.pid ] && kill "$(cat /tmp/hems_keeper.pid)" 2>/dev/null || true
      sleep 0.5
      [ -f /tmp/hems.pid ]        && kill "$(cat /tmp/hems.pid)"        2>/dev/null || true
      ;;
    hp)
      [ -f /tmp/hp_keeper.pid ]   && kill "$(cat /tmp/hp_keeper.pid)"   2>/dev/null || true
      sleep 0.5
      [ -f /tmp/hp.pid ]          && kill "$(cat /tmp/hp.pid)"          2>/dev/null || true
      ;;
    ev)
      [ -f /tmp/ev_keeper.pid ]   && kill "$(cat /tmp/ev_keeper.pid)"   2>/dev/null || true
      sleep 0.5
      [ -f /tmp/ev.pid ]          && kill "$(cat /tmp/ev.pid)"          2>/dev/null || true
      ;;
  esac
}

save_fail_logs() {
  local n=$1
  cp "$HEMS_LOG" "$FAIL_DIR/iter${n}_hems.log" 2>/dev/null || true
  cp "$HP_LOG"   "$FAIL_DIR/iter${n}_hp.log"   2>/dev/null || true
  cp "$EV_LOG"   "$FAIL_DIR/iter${n}_ev.log"   2>/dev/null || true
}

# ---------------------------------------------------------------------------
# Main loop
# ---------------------------------------------------------------------------

echo "Stress test: HEMS + HP + EV  (${ITERATIONS} iterations)"
echo "Fail logs: $FAIL_DIR"
echo ""

for iter in $(seq 1 "$ITERATIONS"); do
  fail_reason=""

  printf "[%02d/%02d] " "$iter" "$ITERATIONS"

  # --- Cleanup from previous iteration ---
  cleanup_iter
  sleep 2

  # --- Start HEMS ---
  rm -f "$HEMS_PIPE"; mkfifo "$HEMS_PIPE"; : > "$HEMS_LOG"
  tail -f /dev/null > "$HEMS_PIPE" &
  echo $! > /tmp/hems_keeper.pid
  "${STDBUF[@]}" "$SCRIPT_DIR/../build/hems$EXE" \
      4710 "$HP_SKI" "$HEMS_CERT" "$HEMS_KEY" auto \
      --remote "$EV_SKI" \
      < "$HEMS_PIPE" >> "$HEMS_LOG" 2>&1 &
  echo $! > /tmp/hems.pid

  # --- Start HP ---
  rm -f "$HP_PIPE"; mkfifo "$HP_PIPE"; : > "$HP_LOG"
  tail -f /dev/null > "$HP_PIPE" &
  echo $! > /tmp/hp_keeper.pid
  "${STDBUF[@]}" "$SCRIPT_DIR/../build/heat_pump$EXE" \
      4712 "$HEMS_SKI" "$HP_CERT" "$HP_KEY" auto \
      < "$HP_PIPE" >> "$HP_LOG" 2>&1 &
  echo $! > /tmp/hp.pid

  # --- Start EV ---
  rm -f "$EV_PIPE"; mkfifo "$EV_PIPE"; : > "$EV_LOG"
  tail -f /dev/null > "$EV_PIPE" &
  echo $! > /tmp/ev_keeper.pid
  "${STDBUF[@]}" "$SCRIPT_DIR/../build/ev_charger$EXE" \
      4714 "$HEMS_SKI" "$EV_CERT" "$EV_KEY" auto \
      < "$EV_PIPE" >> "$EV_LOG" 2>&1 &
  echo $! > /tmp/ev.pid

  # --- Wait for all SHIP connections ---
  connected=0
  for i in $(seq 1 "$CONNECT_TIMEOUT"); do
    hp_ok=0; ev_ok=0; conn_count=0
    grep -aq "Remote SKI connected" "$HP_LOG"   2>/dev/null && hp_ok=1
    grep -aq "Remote SKI connected" "$EV_LOG"   2>/dev/null && ev_ok=1
    conn_count=$(grep -ac "Remote SKI connected" "$HEMS_LOG" 2>/dev/null) || conn_count=0
    if [ "$hp_ok" -eq 1 ] && [ "$ev_ok" -eq 1 ] && [ "$conn_count" -ge 2 ]; then
      connected=1; printf "conn=%ds " "$i"; break
    fi
    sleep 1
  done

  if [ "$connected" -eq 0 ]; then
    printf "FAIL(no-connection)\n"
    save_fail_logs "$iter"
    cleanup_iter
    FAIL=$((FAIL+1))
    continue
  fi

  sleep "$SETTLE_SECS"

  # --- Resolve entity addresses ---
  entity_ok=0
  for i in $(seq 1 10); do
    echo "ma_mpc list" > "$HEMS_PIPE"; sleep 1
    grep -aq "ma_mpc connected remotes (2)" "$HEMS_LOG" 2>/dev/null && { entity_ok=1; break; }
  done

  data_ok=1

  if [ "$entity_ok" -eq 1 ]; then
    HP_ENTITY=$(grep -a -A3 "ma_mpc connected remotes (2):" "$HEMS_LOG" | \
                grep "^  " | grep "NIBE\|HeatPump" | sed 's/^  //' | head -1)
    EV_ENTITY=$(grep -a -A3 "ma_mpc connected remotes (2):" "$HEMS_LOG" | \
                grep "^  " | grep -v "NIBE\|HeatPump" | sed 's/^  //' | head -1)

    # MPC: HP sets 1000, EV sets 2000
    echo "mu_mpc set power_total 1000" > "$HP_PIPE"; sleep 0.3
    echo "mu_mpc set power_total 2000" > "$EV_PIPE"; sleep 0.3
    sleep 2

    # HEMS reads MA MPC from both remotes
    echo "ma_mpc get power_total --remote $HP_ENTITY" > "$HEMS_PIPE"; sleep 0.2
    echo "ma_mpc get power_total --remote $EV_ENTITY" > "$HEMS_PIPE"; sleep 0.2
    sleep 1

    # LPC: HEMS sends 5000 W limit to HP
    echo "eg_lpc set power_limit 5000 PT0S true --remote $HP_ENTITY" > "$HEMS_PIPE"; sleep 0.3
    sleep 2

    # Verify results
    # MA MPC: logged by explicit "ma_mpc get" CLI commands above
    ma_hp=$(grep -a "MA MPC measurement power_total:" "$HEMS_LOG" | sed -n '1p' | grep -o 'value=[^ ]*' | head -1)
    ma_ev=$(grep -a "MA MPC measurement power_total:" "$HEMS_LOG" | sed -n '2p' | grep -o 'value=[^ ]*' | head -1)
    # LPC: HP auto-logs "CS LPC Power Limit received <N>W" when it receives the limit;
    # "CS LPC Active Power Limit:" only appears after an explicit "cs_lpc get" CLI command.
    lpc=$(grep -a "CS LPC Power Limit received" "$HP_LOG" | tail -1 | grep -o '[0-9]*W' | head -1)

    if [ "${ma_hp#value=}" != "1000" ]; then
      data_ok=0; fail_reason="${fail_reason:+$fail_reason,}MA-HP=${ma_hp:-N/A}"
    fi
    if [ "${ma_ev#value=}" != "2000" ]; then
      data_ok=0; fail_reason="${fail_reason:+$fail_reason,}MA-EV=${ma_ev:-N/A}"
    fi
    if [ "$lpc" != "5000W" ]; then
      data_ok=0; fail_reason="${fail_reason:+$fail_reason,}LPC=${lpc:-N/A}"
    fi
  else
    data_ok=0; fail_reason="entity-list-timeout"
  fi

  # --- Exit in random order ---
  read -ra order <<< "$(shuffle_echo hems hp ev)"
  printf "exit[%s] " "${order[*]}"
  for node in "${order[@]}"; do
    exit_node "$node"
    sleep $((RANDOM % 3 + 1))
  done

  if [ "$data_ok" -eq 1 ]; then
    printf "PASS\n"
    PASS=$((PASS+1))
  else
    printf "FAIL(%s)\n" "$fail_reason"
    save_fail_logs "$iter"
    FAIL=$((FAIL+1))
  fi
done

# ---------------------------------------------------------------------------
# Summary
# ---------------------------------------------------------------------------
echo ""
echo "======================================="
printf "  Iterations : %d\n" "$ITERATIONS"
printf "  Passed     : %d\n" "$PASS"
printf "  Failed     : %d\n" "$FAIL"
echo "======================================="
[ "$FAIL" -gt 0 ] && echo "  Failure logs saved to: $FAIL_DIR"

exit "$FAIL"
