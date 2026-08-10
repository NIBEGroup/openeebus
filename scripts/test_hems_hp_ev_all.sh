#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

HP_PIPE=/tmp/hp_pipe
EV_PIPE=/tmp/ev_pipe
HEMS_PIPE=/tmp/hems_pipe
HP_LOG=/tmp/hp.log
EV_LOG=/tmp/ev.log
HEMS_LOG=/tmp/hems.log

# OS detection
case "$(uname -s)" in
  MINGW*|CYGWIN*|MSYS*) EXE=".exe" ;;
  *) EXE="" ;;
esac

STDBUF=()
command -v stdbuf >/dev/null 2>&1 && STDBUF=(stdbuf -oL)

# Credentials — override via environment variables
HP_SKI="${EEBUS_HP_SKI:-40c61c3526f271e8e1547851c46f6ea20d4c6f83}"
HP_CERT="${EEBUS_HP_CERT:-$SCRIPT_DIR/certificates/heat_pump.crt}"
HP_KEY="${EEBUS_HP_KEY:-$SCRIPT_DIR/certificates/heat_pump.key}"

EV_SKI="${EEBUS_EV_SKI:-5a139a8131d0b65b3078878bafd7ff71b84721a4}"
EV_CERT="${EEBUS_EV_CERT:-$SCRIPT_DIR/certificates/ev_charger.crt}"
EV_KEY="${EEBUS_EV_KEY:-$SCRIPT_DIR/certificates/ev_charger.key}"

HEMS_CERT="${EEBUS_HEMS_CERT:-$SCRIPT_DIR/certificates/hems.crt}"
HEMS_KEY="${EEBUS_HEMS_KEY:-$SCRIPT_DIR/certificates/hems.key}"

# Validate EV certificate exists
if [ ! -f "$EV_CERT" ] || [ ! -f "$EV_KEY" ]; then
  echo "ERROR: EV charger certificate not found."
  echo "  Expected: $EV_CERT and $EV_KEY"
  echo "  Generate with: openssl req -x509 -newkey ec -pkeyopt ec_paramgen_curve:P-256 \\"
  echo "    -keyout $EV_KEY -out $EV_CERT -days 3650 -nodes -subj '/CN=ev_charger'"
  exit 1
fi

# -----------------------------------------------------------------------
# Cleanup: kill leftover processes from previous runs
# -----------------------------------------------------------------------
echo "Cleaning up any leftover processes from previous runs..."
for pidfile in /tmp/hems.pid /tmp/hp.pid /tmp/ev.pid \
               /tmp/hems_keeper.pid /tmp/hp_keeper.pid /tmp/ev_keeper.pid; do
  if [ -f "$pidfile" ]; then
    kill "$(cat "$pidfile")" 2>/dev/null || true
    rm -f "$pidfile"
  fi
done
for port in 4710 4712 4714; do
  pids=$(lsof -ti tcp:$port 2>/dev/null || true)
  [ -n "$pids" ] && echo "$pids" | xargs kill -9 2>/dev/null || true
done
sleep 2

# -----------------------------------------------------------------------
# Start HEMS (registers both HP and EV SKIs)
# -----------------------------------------------------------------------
rm -f "$HEMS_PIPE"; mkfifo "$HEMS_PIPE"; : > "$HEMS_LOG"
tail -f /dev/null > "$HEMS_PIPE" &
echo $! > /tmp/hems_keeper.pid

"${STDBUF[@]}" "$SCRIPT_DIR/../build/hems$EXE" \
    4710 "$HP_SKI" "$HEMS_CERT" "$HEMS_KEY" auto \
    --remote "$EV_SKI" \
    < "$HEMS_PIPE" >> "$HEMS_LOG" 2>&1 &
echo $! > /tmp/hems.pid
echo "hems started (PID=$(cat /tmp/hems.pid)), pipe=$HEMS_PIPE, log=$HEMS_LOG"

# -----------------------------------------------------------------------
# Start HP
# -----------------------------------------------------------------------
"$SCRIPT_DIR/hp_pipe.sh"

# -----------------------------------------------------------------------
# Start EV charger
# -----------------------------------------------------------------------
"$SCRIPT_DIR/ev_pipe.sh"

# -----------------------------------------------------------------------
# Wait for both SHIP connections
# -----------------------------------------------------------------------
echo "Waiting for both SHIP connections (up to 120s)..."
for i in $(seq 1 120); do
  hp_ok=0; ev_ok=0; hems_hp_ok=0; hems_ev_ok=0

  grep -aq "Remote SKI connected" "$HP_LOG"   2>/dev/null && hp_ok=1
  grep -aq "Remote SKI connected" "$EV_LOG"   2>/dev/null && ev_ok=1

  conn_count=$(grep -ac "Remote SKI connected" "$HEMS_LOG" 2>/dev/null) || conn_count=0
  [ "$conn_count" -ge 1 ] && hems_hp_ok=1
  [ "$conn_count" -ge 2 ] && hems_ev_ok=1

  if [ "$hp_ok" -eq 1 ] && [ "$ev_ok" -eq 1 ] && \
     [ "$hems_hp_ok" -eq 1 ] && [ "$hems_ev_ok" -eq 1 ]; then
    echo "Both SHIP connections established after ${i}s"; break
  fi
  sleep 1
done

echo "Waiting 5s for use case subscriptions to settle..."
sleep 5

# -----------------------------------------------------------------------
# Extract entity addresses from ma_mpc list (HP connects first → entity 1)
# -----------------------------------------------------------------------
echo ""
echo "=== [List Check] Waiting for ma_mpc list to show 2 remotes ==="
for i in $(seq 1 30); do
  echo "ma_mpc list" > "$HEMS_PIPE"
  sleep 1
  if grep -aq "ma_mpc connected remotes (2)" "$HEMS_LOG" 2>/dev/null; then
    echo "  confirmed after ${i}s"; break
  fi
done
HP_ENTITY=$(grep -a -A3 "ma_mpc connected remotes (2):" "$HEMS_LOG" | grep "^  " | grep "NIBE\|HeatPump" | sed 's/^  //' | head -1)
EV_ENTITY=$(grep -a -A3 "ma_mpc connected remotes (2):" "$HEMS_LOG" | grep "^  " | grep -v "NIBE\|HeatPump" | sed 's/^  //' | head -1)
echo "  HP entity: ${HP_ENTITY:-<not found>}"
echo "  EV entity: ${EV_ENTITY:-<not found>}"

# -----------------------------------------------------------------------
# Scenario 1: HP (MU) sets power — HEMS (MA) reads from HP entity
# -----------------------------------------------------------------------
echo ""
echo "=== [Scenario 1] HP sets MPC power measurements ==="
echo "mu_mpc set power_total   1000" > "$HP_PIPE"; sleep 0.3
echo "mu_mpc set power_phase_a 1100" > "$HP_PIPE"; sleep 0.3
echo "mu_mpc set power_phase_b 1200" > "$HP_PIPE"; sleep 0.3
echo "mu_mpc set power_phase_c 1300" > "$HP_PIPE"; sleep 0.3

echo "Waiting 3s for measurements to propagate..."
sleep 3

echo "mu_mpc get power_total"   > "$HP_PIPE"; sleep 0.2
echo "mu_mpc get power_phase_a" > "$HP_PIPE"; sleep 0.2
echo "mu_mpc get power_phase_b" > "$HP_PIPE"; sleep 0.2
echo "mu_mpc get power_phase_c" > "$HP_PIPE"; sleep 0.2
echo "ma_mpc get power_total   --remote $HP_ENTITY" > "$HEMS_PIPE"; sleep 0.2
echo "ma_mpc get power_phase_a --remote $HP_ENTITY" > "$HEMS_PIPE"; sleep 0.2
echo "ma_mpc get power_phase_b --remote $HP_ENTITY" > "$HEMS_PIPE"; sleep 0.2
echo "ma_mpc get power_phase_c --remote $HP_ENTITY" > "$HEMS_PIPE"; sleep 0.2
sleep 2

# -----------------------------------------------------------------------
# Scenario 2: EV (MU) sets power — HEMS (MA) reads from EV entity
# -----------------------------------------------------------------------
echo ""
echo "=== [Scenario 2] EV sets MPC power measurements ==="
echo "mu_mpc set power_total   2000" > "$EV_PIPE"; sleep 0.3
echo "mu_mpc set power_phase_a 2100" > "$EV_PIPE"; sleep 0.3
echo "mu_mpc set power_phase_b 2200" > "$EV_PIPE"; sleep 0.3
echo "mu_mpc set power_phase_c 2300" > "$EV_PIPE"; sleep 0.3

echo "Waiting 3s for measurements to propagate..."
sleep 3

echo "mu_mpc get power_total"   > "$EV_PIPE";   sleep 0.2
echo "mu_mpc get power_phase_a" > "$EV_PIPE";   sleep 0.2
echo "mu_mpc get power_phase_b" > "$EV_PIPE";   sleep 0.2
echo "mu_mpc get power_phase_c" > "$EV_PIPE";   sleep 0.2
echo "ma_mpc get power_total   --remote $EV_ENTITY" > "$HEMS_PIPE"; sleep 0.2
echo "ma_mpc get power_phase_a --remote $EV_ENTITY" > "$HEMS_PIPE"; sleep 0.2
echo "ma_mpc get power_phase_b --remote $EV_ENTITY" > "$HEMS_PIPE"; sleep 0.2
echo "ma_mpc get power_phase_c --remote $EV_ENTITY" > "$HEMS_PIPE"; sleep 0.2
sleep 2

# -----------------------------------------------------------------------
# Scenario 3: LPC — HEMS sends power limit to HP
# -----------------------------------------------------------------------
echo ""
echo "=== [Scenario 3] HEMS sends LPC power limit to HP ==="
echo "eg_lpc set power_limit 5000 PT0S true --remote $HP_ENTITY" > "$HEMS_PIPE"; sleep 0.3

echo "Waiting 3s for limit to propagate to HP..."
sleep 3

echo "cs_lpc get power_limit" > "$HP_PIPE"; sleep 0.2
sleep 1

# -----------------------------------------------------------------------
# Scenario 4: EV disconnect — HEMS + HP continue unaffected
# -----------------------------------------------------------------------
echo ""
echo "=== [Scenario 4] EV disconnect — HP+HEMS continue unaffected ==="
echo "exit" > "$EV_PIPE" || true
sleep 1
kill "$(cat /tmp/ev_keeper.pid)" 2>/dev/null || true

EV_DISC_VERIFIED=0
for i in $(seq 1 15); do
  if ! kill -0 "$(cat /tmp/ev.pid)" 2>/dev/null; then
    echo "  EV process exited after ${i}s"; EV_DISC_VERIFIED=1; break
  fi
  sleep 1
done
[ "$EV_DISC_VERIFIED" -eq 0 ] && echo "  WARNING: EV process did not exit in time"
sleep 3

echo "Sending new LPC limit to HP via HEMS (post-EV-disconnect)..."
echo "eg_lpc set power_limit 7000 PT0S true" > "$HEMS_PIPE"; sleep 0.3
echo "Waiting 3s for limit to propagate..."
sleep 3
echo "cs_lpc get power_limit" > "$HP_PIPE"; sleep 0.2
sleep 1

# -----------------------------------------------------------------------
# Helpers
# -----------------------------------------------------------------------
mu_get_hp()    { grep -a "MU MPC measurement $1:"   "$HP_LOG"   | tail -1 | grep -o 'value=[^ ]*' | head -1; }
mu_get_ev()    { grep    "MU MPC measurement $1:"   "$EV_LOG"   | tail -1 | grep -o 'value=[^ ]*' | head -1; }
ma_get_hp()    { grep -a "MA MPC measurement $1:"   "$HEMS_LOG" | sed -n '1p' | grep -o 'value=[^ ]*' | head -1; }
ma_get_ev()    { grep -a "MA MPC measurement $1:"   "$HEMS_LOG" | sed -n '2p' | grep -o 'value=[^ ]*' | head -1; }
lpc_get()      { grep -a "CS LPC Active Power Limit:" "$HP_LOG" | sed -n '1p' | grep -o 'value=[^ ,]*' | head -1; }
lpc_post_get() { grep -a "CS LPC Active Power Limit:" "$HP_LOG" | tail -1    | grep -o 'value=[^ ,]*' | head -1; }

# -----------------------------------------------------------------------
# Print text results
# -----------------------------------------------------------------------
echo ""
echo "======================================================================"
echo "  HEMS + HP + EV Integration Test"
echo "======================================================================"
printf "%-22s  %12s  %12s  %12s  %12s  %12s\n" "Measurement" "HP set" "HP get" "EV get" "MA-HP" "MA-EV"
printf "%-22s  %12s  %12s  %12s  %12s  %12s\n" \
  "----------------------" "------------" "------------" "------------" "------------" "------------"

row() {
  local name=$1 hp_set=$2 ev_set=$3
  local hp_mu ev_mu ma_hp ma_ev
  hp_mu=$(mu_get_hp "$name"); ev_mu=$(mu_get_ev "$name")
  ma_hp=$(ma_get_hp "$name"); ma_ev=$(ma_get_ev "$name")
  printf "%-22s  %12s  %12s  %12s  %12s  %12s\n" \
    "$name" "value=$hp_set" "${hp_mu:-N/A}" "${ev_mu:-N/A}" "${ma_hp:-N/A}" "${ma_ev:-N/A}"
}

row power_total   1000 2000
row power_phase_a 1100 2100
row power_phase_b 1200 2200
row power_phase_c 1300 2300

echo ""
echo "=== LPC (Scenario 3) ==="
printf "%-22s  %12s  %12s\n" "Measurement" "HEMS set" "HP get"
printf "%-22s  %12s  %12s\n" "----------------------" "------------" "------------"
_lpc=$(lpc_get); printf "%-22s  %12s  %12s\n" "power_limit" "value=5000" "${_lpc:-N/A}"

echo ""
echo "=== EV disconnect + LPC (Scenario 4) ==="
printf "%-22s  %12s\n" "EV disconnected" "$([ "$EV_DISC_VERIFIED" -eq 1 ] && echo PASS || echo FAIL)"
_lpc_post=$(lpc_post_get); printf "%-22s  %12s  %12s\n" "power_limit post" "value=7000" "${_lpc_post:-N/A}"

# -----------------------------------------------------------------------
# Shut down all nodes
# -----------------------------------------------------------------------
echo ""
echo "Sending exit to all nodes..."
echo "exit" > "$HP_PIPE"   || true
echo "exit" > "$HEMS_PIPE" || true
sleep 1
kill "$(cat /tmp/hp.pid)"         2>/dev/null || true
kill "$(cat /tmp/hems.pid)"       2>/dev/null || true
kill "$(cat /tmp/hp_keeper.pid)"   2>/dev/null || true
kill "$(cat /tmp/hems_keeper.pid)" 2>/dev/null || true

# -----------------------------------------------------------------------
# HTML report
# -----------------------------------------------------------------------
HTML_REPORT=/tmp/eebus_hems_hp_ev_report.html
TIMESTAMP=$(date "+%Y-%m-%d %H:%M:%S")
PASS_COUNT=0; FAIL_COUNT=0; NA_COUNT=0

html_row() {
  local name=$1 hp_set=$2 ev_set=$3
  local hp_mu ev_mu ma_hp ma_ev
  local hp_css ev_css ma_hp_css ma_ev_css
  hp_mu=$(mu_get_hp "$name"); ev_mu=$(mu_get_ev "$name")
  ma_hp=$(ma_get_hp "$name"); ma_ev=$(ma_get_ev "$name")

  # Check HP MU vs set
  if   [ -z "$hp_mu" ];                   then hp_css="na";   NA_COUNT=$((NA_COUNT+1))
  elif [ "${hp_mu#value=}" = "$hp_set" ]; then hp_css="pass"; PASS_COUNT=$((PASS_COUNT+1))
  else hp_css="fail"; FAIL_COUNT=$((FAIL_COUNT+1)); fi

  # Check EV MU vs set
  if   [ -z "$ev_mu" ];                   then ev_css="na";   NA_COUNT=$((NA_COUNT+1))
  elif [ "${ev_mu#value=}" = "$ev_set" ]; then ev_css="pass"; PASS_COUNT=$((PASS_COUNT+1))
  else ev_css="fail"; FAIL_COUNT=$((FAIL_COUNT+1)); fi

  # Check HEMS MA-HP vs HP set
  if   [ -z "$ma_hp" ];                   then ma_hp_css="na";   NA_COUNT=$((NA_COUNT+1))
  elif [ "${ma_hp#value=}" = "$hp_set" ]; then ma_hp_css="pass"; PASS_COUNT=$((PASS_COUNT+1))
  else ma_hp_css="fail"; FAIL_COUNT=$((FAIL_COUNT+1)); fi

  # Check HEMS MA-EV vs EV set
  if   [ -z "$ma_ev" ];                   then ma_ev_css="na";   NA_COUNT=$((NA_COUNT+1))
  elif [ "${ma_ev#value=}" = "$ev_set" ]; then ma_ev_css="pass"; PASS_COUNT=$((PASS_COUNT+1))
  else ma_ev_css="fail"; FAIL_COUNT=$((FAIL_COUNT+1)); fi

  echo "      <tr>"
  echo "        <td>${name//_/ }</td>"
  echo "        <td>$hp_set</td>"
  echo "        <td class=\"$hp_css\">${hp_mu:-N/A}</td>"
  echo "        <td>$ev_set</td>"
  echo "        <td class=\"$ev_css\">${ev_mu:-N/A}</td>"
  echo "        <td class=\"$ma_hp_css\">${ma_hp:-N/A}</td>"
  echo "        <td class=\"$ma_ev_css\">${ma_ev:-N/A}</td>"
  echo "      </tr>"
}

lpc_raw=$(lpc_get)
lpc_num="${lpc_raw#value=}"
if   [ -z "$lpc_num" ];       then lpc_css="na";   NA_COUNT=$((NA_COUNT+1))
elif [ "$lpc_num" = "5000" ]; then lpc_css="pass"; PASS_COUNT=$((PASS_COUNT+1))
else lpc_css="fail"; FAIL_COUNT=$((FAIL_COUNT+1)); fi

lpc_post_raw=$(lpc_post_get)
lpc_post_num="${lpc_post_raw#value=}"
if   [ -z "$lpc_post_num" ];   then lpc_post_css="na";   NA_COUNT=$((NA_COUNT+1))
elif [ "$lpc_post_num" = "7000" ]; then lpc_post_css="pass"; PASS_COUNT=$((PASS_COUNT+1))
else lpc_post_css="fail"; FAIL_COUNT=$((FAIL_COUNT+1)); fi

ev_disc_css=$([ "$EV_DISC_VERIFIED" -eq 1 ] && echo "pass" || echo "fail")
[ "$EV_DISC_VERIFIED" -eq 1 ] && PASS_COUNT=$((PASS_COUNT+1)) || FAIL_COUNT=$((FAIL_COUNT+1))

{
cat <<'HTMLHEAD'
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>OpenEEBUS HEMS+HP+EV Integration Test</title>
  <style>
    *, *::before, *::after { box-sizing: border-box; }
    body { font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif;
           background: #f4f6f8; margin: 0; padding: 2em; color: #2c3e50; }
    h1   { margin: 0 0 0.2em; font-size: 1.6em; }
    .meta { color: #7f8c8d; font-size: 0.9em; margin-bottom: 2em; }
    h2   { font-size: 1.1em; margin: 2em 0 0.6em; color: #34495e;
           border-left: 4px solid #3498db; padding-left: 0.6em; }
    table { border-collapse: collapse; width: 100%; max-width: 900px;
            background: #fff; border-radius: 6px;
            box-shadow: 0 1px 4px rgba(0,0,0,.12); }
    thead th { background: #2c3e50; color: #fff; padding: 9px 14px;
               text-align: left; font-weight: 600; font-size: 0.88em; }
    thead th:first-child { border-radius: 6px 0 0 0; }
    thead th:last-child  { border-radius: 0 6px 0 0; }
    tbody td { padding: 7px 14px; font-size: 0.9em; border-bottom: 1px solid #ecf0f1; }
    tbody tr:last-child td { border-bottom: none; }
    td.pass { color: #27ae60; font-weight: 600; }
    td.fail { color: #e74c3c; font-weight: 600; background: #fdf2f2; }
    td.na   { color: #e67e22; }
    .summary { display: inline-flex; gap: 1.5em; margin-top: 2.5em;
               background: #fff; padding: 1em 1.5em; border-radius: 6px;
               box-shadow: 0 1px 4px rgba(0,0,0,.12); }
    .s-label { font-size: 0.82em; color: #7f8c8d; display: block; }
    .s-val   { font-size: 1.6em; font-weight: 700; }
    .s-pass  { color: #27ae60; }
    .s-fail  { color: #e74c3c; }
    .s-na    { color: #e67e22; }
    .overall { margin-top: 1.2em; font-size: 1em; font-weight: 600; }
    .overall.ok  { color: #27ae60; }
    .overall.nok { color: #e74c3c; }
  </style>
</head>
<body>
HTMLHEAD

echo "  <h1>OpenEEBUS HEMS + HP + EV Integration Test</h1>"
echo "  <div class=\"meta\">Generated: $TIMESTAMP &nbsp;|&nbsp; Three-node scenario: HEMS (MA) + heat_pump (MU) + ev_charger (MU)</div>"

echo "  <h2>Scenario 1 &amp; 2 — Simultaneous MPC Measurements (HP and EV)</h2>"
echo "  <table>"
echo "    <thead><tr><th>Measurement</th><th>HP set</th><th>HP get</th><th>EV set</th><th>EV get</th><th>HEMS MA-HP</th><th>HEMS MA-EV</th></tr></thead>"
echo "    <tbody>"
html_row power_total   1000 2000
html_row power_phase_a 1100 2100
html_row power_phase_b 1200 2200
html_row power_phase_c 1300 2300
echo "    </tbody>"
echo "  </table>"

echo "  <h2>Scenario 3 — LPC: HEMS sends power limit to HP</h2>"
echo "  <table>"
echo "    <thead><tr><th>Measurement</th><th>HEMS set</th><th>HP get</th></tr></thead>"
echo "    <tbody>"
echo "      <tr><td>power limit</td><td>5000</td><td class=\"$lpc_css\">${lpc_num:-N/A}</td></tr>"
echo "    </tbody>"
echo "  </table>"

echo "  <h2>Scenario 4 — EV disconnect: HEMS + HP continue unaffected</h2>"
echo "  <table>"
echo "    <thead><tr><th>Check</th><th>Expected</th><th>Result</th></tr></thead>"
echo "    <tbody>"
echo "      <tr><td>EV process exited</td><td>yes</td><td class=\"$ev_disc_css\">$([ "$EV_DISC_VERIFIED" -eq 1 ] && echo PASS || echo FAIL)</td></tr>"
echo "      <tr><td>LPC power limit (post-disconnect)</td><td>7000</td><td class=\"$lpc_post_css\">${lpc_post_num:-N/A}</td></tr>"
echo "    </tbody>"
echo "  </table>"

overall_css="ok"; overall_text="ALL PASSED"
if   [ "$FAIL_COUNT" -gt 0 ]; then overall_css="nok"; overall_text="FAILED ($FAIL_COUNT failure(s))"
elif [ "$NA_COUNT"   -gt 0 ]; then overall_css="nok"; overall_text="INCOMPLETE ($NA_COUNT measurement(s) missing)"
fi

cat <<HTMLFOOT
  <div class="summary">
    <div><span class="s-label">Passed</span><span class="s-val s-pass">$PASS_COUNT</span></div>
    <div><span class="s-label">Failed</span><span class="s-val s-fail">$FAIL_COUNT</span></div>
    <div><span class="s-label">N/A</span>   <span class="s-val s-na">$NA_COUNT</span></div>
  </div>
  <div class="overall $overall_css">Overall: $overall_text</div>
</body>
</html>
HTMLFOOT

} > "$HTML_REPORT"

echo "HTML report written to $HTML_REPORT"
open "$HTML_REPORT" 2>/dev/null || true
