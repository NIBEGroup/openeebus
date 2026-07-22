#!/usr/bin/env bash
# Measures EebusService close duration over up to 50 iterations.
# Each iteration: start both nodes,
# establish SHIP, do minimal LPC exchange, send "exit", wait for both processes
# to die, extract CLOSE_DURATION_MS printed by the debug instrumentation.
#
# Build requirements (must be set at compile time):
#   EEBUS_SERVICE_DEBUG=1  (src/service/service/eebus_service.c)
#     Enables EebusService::Stop(): begin / Destruct(): end prints used to
#     compute the close duration from DebugPrintf timestamps.
#   SHIP_NODE_DEBUG=1  (src/ship/ship_node/ship_node.c)
#     Enables ShipNode::Stop(): / Destruct(): prints used to report the last
#     shutdown step reached per iteration.

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/../build"

# OS detection: add .exe suffix on Windows (Git Bash / MSYS2 / Cygwin)
case "$(uname -s)" in
  MINGW*|CYGWIN*|MSYS*) EXE=".exe" ;;
  *) EXE="" ;;
esac

# stdbuf is part of GNU coreutils; available on Linux, macOS (brew), and Git Bash
STDBUF=()
command -v stdbuf >/dev/null 2>&1 && STDBUF=(stdbuf -oL)

HP_PIPE=/tmp/hp_pipe
HEMS_PIPE=/tmp/hems_pipe
HP_LOG=/tmp/hp.log
HEMS_LOG=/tmp/hems.log

# Credentials — override via environment variables for other machines / OSes
HP_SKI="${EEBUS_HP_SKI:-1bb991d59a94cc1925486be3addb07200b9d7680}"
HP_CERT="${EEBUS_HP_CERT:-$SCRIPT_DIR/certificates/heat_pump.crt}"
HP_KEY="${EEBUS_HP_KEY:-$SCRIPT_DIR/certificates/heat_pump.key}"

HEMS_SKI="${EEBUS_HEMS_SKI:-40c61c3526f271e8e1547851c46f6ea20d4c6f83}"
HEMS_CERT="${EEBUS_HEMS_CERT:-$SCRIPT_DIR/certificates/hems.crt}"
HEMS_KEY="${EEBUS_HEMS_KEY:-$SCRIPT_DIR/certificates/hems.key}"

MAX_ITER=${1:-50}
CONNECT_TIMEOUT=60   # seconds to wait for SHIP connection per iteration
CLOSE_TIMEOUT=60     # seconds to wait for process death after exit

HP_TIMES=()
HEMS_TIMES=()
STATUSES=()

start_nodes() {
  rm -f "$HP_PIPE" "$HEMS_PIPE"
  mkfifo "$HP_PIPE" "$HEMS_PIPE"
  : > "$HP_LOG"
  : > "$HEMS_LOG"

  tail -f /dev/null > "$HP_PIPE" &
  echo $! > /tmp/hp_keeper.pid
  tail -f /dev/null > "$HEMS_PIPE" &
  echo $! > /tmp/hems_keeper.pid

  "${STDBUF[@]}" "$BUILD_DIR/heat_pump$EXE" 4712 "$HP_SKI" "$HP_CERT" "$HP_KEY" auto \
      < "$HP_PIPE" >> "$HP_LOG" 2>&1 &
  echo $! > /tmp/hp.pid

  "${STDBUF[@]}" "$BUILD_DIR/hems$EXE" 4710 "$HEMS_SKI" "$HEMS_CERT" "$HEMS_KEY" auto \
      < "$HEMS_PIPE" >> "$HEMS_LOG" 2>&1 &
  echo $! > /tmp/hems.pid

  echo "  heat_pump PID=$(cat /tmp/hp.pid)  hems PID=$(cat /tmp/hems.pid)"
}

kill_all() {
  kill -9 "$(cat /tmp/hp.pid)"       2>/dev/null || true
  kill -9 "$(cat /tmp/hems.pid)"     2>/dev/null || true
  kill -9 "$(cat /tmp/hp_keeper.pid)"   2>/dev/null || true
  kill -9 "$(cat /tmp/hems_keeper.pid)" 2>/dev/null || true
}

# Poll until PID is gone or timeout expires. Returns 0 if gone, 1 if timed out.
wait_pid_dead() {
  local pid=$1 limit=$2 i
  for i in $(seq 1 "$limit"); do
    kill -0 "$pid" 2>/dev/null || return 0
    sleep 1
  done
  return 1
}

# Compute total close duration: EebusService::Stop(): begin → Destruct(): end.
# DebugPrintf uses lwsl_timestamp which prints "[YYYY/MM/DD HH:MM:SS:TTTT]"
# where TTTT = tv_usec/100 (0–9999, units of 0.1 ms).
# Multipliers: HH×36 000 000, MM×600 000, SS×10 000, TTTT×1 — all in 0.1 ms.
# Divide by 10 at the end to get whole milliseconds; handle midnight rollover.
close_duration_ms() {
  awk '
    /EebusService::Stop\(\): begin/ {
      match($0, /[0-9]+:[0-9]+:[0-9]+:[0-9]+/)
      ts = substr($0, RSTART, RLENGTH)
      n = split(ts, a, ":")
      if (n == 4) beg = a[1]*36000000 + a[2]*600000 + a[3]*10000 + a[4]
    }
    /EebusService::Destruct\(\): end/ {
      match($0, /[0-9]+:[0-9]+:[0-9]+:[0-9]+/)
      ts = substr($0, RSTART, RLENGTH)
      n = split(ts, a, ":")
      if (n == 4) fin = a[1]*36000000 + a[2]*600000 + a[3]*10000 + a[4]
    }
    END {
      if (beg && fin) {
        d = fin - beg
        if (d < 0) d += 864000000
        print int(d / 10)
      }
    }
  ' "$1"
}

for iter in $(seq 1 "$MAX_ITER"); do
  echo ""
  echo "=== Iteration $iter / $MAX_ITER ==="

  start_nodes

  HP_PID=$(cat /tmp/hp.pid)
  HEMS_PID=$(cat /tmp/hems.pid)

  # ---- wait for SHIP connection ----
  echo "  Waiting for SHIP connection (up to ${CONNECT_TIMEOUT}s)..."
  connected=false
  for i in $(seq 1 "$CONNECT_TIMEOUT"); do
    if grep -q "Remote SKI connected" "$HP_LOG"   2>/dev/null && \
       grep -q "Remote SKI connected" "$HEMS_LOG" 2>/dev/null; then
      echo "  Connected after ${i}s"
      connected=true
      break
    fi
    sleep 1
  done

  if [ "$connected" = false ]; then
    echo "  ERROR: connection timeout — skipping iteration"
    STATUSES+=("CONN_TIMEOUT")
    HP_TIMES+=("N/A")
    HEMS_TIMES+=("N/A")
    kill_all
    sleep 3
    continue
  fi

  # ---- let use-case subscriptions settle ----
  sleep 3

  # ---- minimal LPC data exchange ----
  echo "  LPC exchange..."
  echo "cs_lpc set failsafe_limit 5000 true"    > "$HP_PIPE";   sleep 0.3
  echo "cs_lpc set failsafe_duration PT2H true" > "$HP_PIPE";   sleep 0.3
  sleep 2
  echo "eg_lpc set power_limit 7000 PT0S true"  > "$HEMS_PIPE"; sleep 0.3
  sleep 2

  # ---- trigger close ----
  echo "  Sending exit..."
  echo "exit" > "$HP_PIPE"   || true
  echo "exit" > "$HEMS_PIPE" || true

  # ---- wait for both processes to die ----
  hp_died=true
  hems_died=true
  wait_pid_dead "$HP_PID"   "$CLOSE_TIMEOUT" || hp_died=false
  wait_pid_dead "$HEMS_PID" "$CLOSE_TIMEOUT" || hems_died=false

  kill -9 "$(cat /tmp/hp_keeper.pid)" "$(cat /tmp/hems_keeper.pid)" 2>/dev/null || true

  iter_status="OK"
  if [ "$hp_died" = false ] || [ "$hems_died" = false ]; then
    echo "  WARNING: process(es) did not exit within ${CLOSE_TIMEOUT}s, force-killing"
    kill -9 "$HP_PID" "$HEMS_PID" 2>/dev/null || true
    iter_status="FORCE_KILL"
  fi
  STATUSES+=("$iter_status")

  # ---- extract close durations ----
  hp_ms=$(  close_duration_ms "$HP_LOG")
  hems_ms=$(close_duration_ms "$HEMS_LOG")

  HP_TIMES+=("${hp_ms:-N/A}")
  HEMS_TIMES+=("${hems_ms:-N/A}")

  echo "  RESULT heat_pump=${hp_ms:-N/A}ms  hems=${hems_ms:-N/A}ms  status=$iter_status"

  # Save logs for post-mortem; show last ship_node step reached
  saved_hems="/tmp/hems_na_iter${iter}.log"
  saved_hp="/tmp/hp_na_iter${iter}.log"
  cp "$HEMS_LOG" "$saved_hems"
  cp "$HP_LOG"   "$saved_hp"
  last_hems=$(grep -iE "stop:|destruct:" "$saved_hems" | tail -1)
  last_hp=$(grep -iE "stop:|destruct:" "$saved_hp"   | tail -1)
  echo "  logs saved  hems last: ${last_hems:-<none>}"
  echo "              hp   last: ${last_hp:-<none>}"

  sleep 3
done

# -----------------------------------------------------------------------
# Summary table
# -----------------------------------------------------------------------
echo ""
echo "========================================================================"
echo "  Close-Timing Summary  (${#HP_TIMES[@]} iterations)"
echo "========================================================================"
printf "%-6s  %14s  %14s  %12s\n" "Iter" "heat_pump (ms)" "hems (ms)" "Status"
printf "%-6s  %14s  %14s  %12s\n" "------" "--------------" "--------------" "------------"
for i in $(seq 0 $((${#HP_TIMES[@]} - 1))); do
  printf "%-6s  %14s  %14s  %12s\n" \
    "$((i+1))" "${HP_TIMES[$i]}" "${HEMS_TIMES[$i]}" "${STATUSES[$i]}"
done

stats() {
  local label=$1; shift
  local vals=("$@")
  local count=0 sum=0 min="" max=""
  for v in "${vals[@]}"; do
    [ "$v" = "N/A" ] && continue
    count=$((count + 1))
    sum=$((sum + v))
    { [ -z "$min" ] || [ "$v" -lt "$min" ]; } && min=$v
    { [ -z "$max" ] || [ "$v" -gt "$max" ]; } && max=$v
  done
  if [ "$count" -gt 0 ]; then
    local avg=$((sum / count))
    printf "  %-14s  n=%-4d  min=%-6d  avg=%-6d  max=%d ms\n" \
      "$label" "$count" "$min" "$avg" "$max"
  else
    printf "  %-14s  no numeric data\n" "$label"
  fi
}

echo ""
stats "heat_pump" "${HP_TIMES[@]}"
stats "hems"      "${HEMS_TIMES[@]}"
echo "========================================================================"
