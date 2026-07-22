#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
HP_PIPE=/tmp/hp_pipe
HEMS_PIPE=/tmp/hems_pipe
HP_LOG=/tmp/hp.log
HEMS_LOG=/tmp/hems.log

"$SCRIPT_DIR/hp_pipe.sh"
"$SCRIPT_DIR/hems_pipe.sh"

echo "Waiting for SHIP connection (up to 120s)..."
for i in $(seq 1 120); do
  if grep -q "Remote SKI connected" "$HP_LOG"   2>/dev/null && \
     grep -q "Remote SKI connected" "$HEMS_LOG" 2>/dev/null; then
    echo "SHIP connected after ${i}s"; break
  fi
  sleep 1
done

echo "Waiting 5s for use case subscriptions to settle..."
sleep 5

# -----------------------------------------------------------------------
# MPC: set all measurements on heat_pump (MU)
# -----------------------------------------------------------------------
echo "=== Setting MPC measurements on heat_pump (MU) ==="

echo "mu_mpc set power_total       100" > "$HP_PIPE"; sleep 0.3
echo "mu_mpc set power_phase_a     110" > "$HP_PIPE"; sleep 0.3
echo "mu_mpc set power_phase_b     120" > "$HP_PIPE"; sleep 0.3
echo "mu_mpc set power_phase_c     130" > "$HP_PIPE"; sleep 0.3
echo "mu_mpc set energy_consumed   200" > "$HP_PIPE"; sleep 0.3
echo "mu_mpc set energy_produced   210" > "$HP_PIPE"; sleep 0.3
echo "mu_mpc set current_phase_a    15" > "$HP_PIPE"; sleep 0.3
echo "mu_mpc set current_phase_b    16" > "$HP_PIPE"; sleep 0.3
echo "mu_mpc set current_phase_c    17" > "$HP_PIPE"; sleep 0.3
echo "mu_mpc set voltage_phase_a   230" > "$HP_PIPE"; sleep 0.3
echo "mu_mpc set voltage_phase_b   231" > "$HP_PIPE"; sleep 0.3
echo "mu_mpc set voltage_phase_c   232" > "$HP_PIPE"; sleep 0.3
echo "mu_mpc set voltage_phase_ab  400" > "$HP_PIPE"; sleep 0.3
echo "mu_mpc set voltage_phase_bc  401" > "$HP_PIPE"; sleep 0.3
echo "mu_mpc set voltage_phase_ac  402" > "$HP_PIPE"; sleep 0.3
echo "mu_mpc set frequency          50" > "$HP_PIPE"; sleep 0.3

echo "Waiting 3s for measurements to propagate to HEMS..."
sleep 3

# -----------------------------------------------------------------------
# Read back from MU (heat_pump) and MA (HEMS)
# -----------------------------------------------------------------------
echo "=== Reading back MPC values from heat_pump (MU) ==="
echo "mu_mpc get power_total"      > "$HP_PIPE"; sleep 0.2
echo "mu_mpc get power_phase_a"    > "$HP_PIPE"; sleep 0.2
echo "mu_mpc get power_phase_b"    > "$HP_PIPE"; sleep 0.2
echo "mu_mpc get power_phase_c"    > "$HP_PIPE"; sleep 0.2
echo "mu_mpc get energy_consumed"  > "$HP_PIPE"; sleep 0.2
echo "mu_mpc get energy_produced"  > "$HP_PIPE"; sleep 0.2
echo "mu_mpc get current_phase_a"  > "$HP_PIPE"; sleep 0.2
echo "mu_mpc get current_phase_b"  > "$HP_PIPE"; sleep 0.2
echo "mu_mpc get current_phase_c"  > "$HP_PIPE"; sleep 0.2
echo "mu_mpc get voltage_phase_a"  > "$HP_PIPE"; sleep 0.2
echo "mu_mpc get voltage_phase_b"  > "$HP_PIPE"; sleep 0.2
echo "mu_mpc get voltage_phase_c"  > "$HP_PIPE"; sleep 0.2
echo "mu_mpc get voltage_phase_ab" > "$HP_PIPE"; sleep 0.2
echo "mu_mpc get voltage_phase_bc" > "$HP_PIPE"; sleep 0.2
echo "mu_mpc get voltage_phase_ac" > "$HP_PIPE"; sleep 0.2
echo "mu_mpc get frequency"        > "$HP_PIPE"; sleep 0.2

echo "=== Reading back MPC values from HEMS (MA) ==="
echo "ma_mpc get power_total"      > "$HEMS_PIPE"; sleep 0.2
echo "ma_mpc get power_phase_a"    > "$HEMS_PIPE"; sleep 0.2
echo "ma_mpc get power_phase_b"    > "$HEMS_PIPE"; sleep 0.2
echo "ma_mpc get power_phase_c"    > "$HEMS_PIPE"; sleep 0.2
echo "ma_mpc get energy_consumed"  > "$HEMS_PIPE"; sleep 0.2
echo "ma_mpc get energy_produced"  > "$HEMS_PIPE"; sleep 0.2
echo "ma_mpc get current_phase_a"  > "$HEMS_PIPE"; sleep 0.2
echo "ma_mpc get current_phase_b"  > "$HEMS_PIPE"; sleep 0.2
echo "ma_mpc get current_phase_c"  > "$HEMS_PIPE"; sleep 0.2
echo "ma_mpc get voltage_phase_a"  > "$HEMS_PIPE"; sleep 0.2
echo "ma_mpc get voltage_phase_b"  > "$HEMS_PIPE"; sleep 0.2
echo "ma_mpc get voltage_phase_c"  > "$HEMS_PIPE"; sleep 0.2
echo "ma_mpc get voltage_phase_ab" > "$HEMS_PIPE"; sleep 0.2
echo "ma_mpc get voltage_phase_bc" > "$HEMS_PIPE"; sleep 0.2
echo "ma_mpc get voltage_phase_ac" > "$HEMS_PIPE"; sleep 0.2
echo "ma_mpc get frequency"        > "$HEMS_PIPE"; sleep 0.2

sleep 2

# -----------------------------------------------------------------------
# Helpers
# -----------------------------------------------------------------------
mu_get() { grep "MU MPC measurement $1:" "$HP_LOG"   | tail -1 | grep -o 'value=[^ ]*' | head -1; }
ma_get() { grep "MA MPC measurement $1:" "$HEMS_LOG" | tail -1 | grep -o 'value=[^ ]*' | head -1; }

# -----------------------------------------------------------------------
# Print text results
# -----------------------------------------------------------------------
echo ""
echo "======================================================================"
echo "  MPC Integration Test — All Scenarios"
echo "======================================================================"
printf "%-22s  %12s  %12s  %12s\n" "Measurement" "MU set" "MU get" "MA get"
printf "%-22s  %12s  %12s  %12s\n" \
  "----------------------" "------------" "------------" "------------"

mpc_row() {
  local name=$1 set_val=$2 mu_val ma_val
  mu_val=$(mu_get "$name"); ma_val=$(ma_get "$name")
  printf "%-22s  %12s  %12s  %12s\n" \
    "$name" "value=$set_val" "${mu_val:-N/A}" "${ma_val:-N/A}"
}

mpc_row power_total      100
mpc_row power_phase_a    110
mpc_row power_phase_b    120
mpc_row power_phase_c    130
mpc_row energy_consumed  200
mpc_row energy_produced  210
mpc_row current_phase_a   15
mpc_row current_phase_b   16
mpc_row current_phase_c   17
mpc_row voltage_phase_a  230
mpc_row voltage_phase_b  231
mpc_row voltage_phase_c  232
mpc_row voltage_phase_ab 400
mpc_row voltage_phase_bc 401
mpc_row voltage_phase_ac 402
mpc_row frequency         50

echo "Sending exit to both nodes..."
echo "exit" > "$HP_PIPE"   || true
echo "exit" > "$HEMS_PIPE" || true
sleep 1
kill "$(cat /tmp/hp_keeper.pid)" "$(cat /tmp/hems_keeper.pid)" 2>/dev/null || true

# -----------------------------------------------------------------------
# HTML report
# -----------------------------------------------------------------------
HTML_REPORT=/tmp/eebus_mpc_test_report.html
TIMESTAMP=$(date "+%Y-%m-%d %H:%M:%S")
PASS_COUNT=0; FAIL_COUNT=0; NA_COUNT=0

html_mpc_row() {
  local name=$1 set_val=$2
  local mu_raw ma_raw mu_num ma_num status css
  mu_raw=$(mu_get "$name"); ma_raw=$(ma_get "$name")
  mu_num="${mu_raw#value=}"; mu_num="${mu_num:-N/A}"
  ma_num="${ma_raw#value=}"; ma_num="${ma_num:-N/A}"
  if   [ "$mu_num" = "N/A" ] || [ "$ma_num" = "N/A" ]; then
    status="N/A"; css="na"; NA_COUNT=$((NA_COUNT+1))
  elif [ "$mu_num" = "$set_val" ] && [ "$ma_num" = "$set_val" ]; then
    status="PASS"; css="pass"; PASS_COUNT=$((PASS_COUNT+1))
  else
    status="FAIL"; css="fail"; FAIL_COUNT=$((FAIL_COUNT+1))
  fi
  echo "      <tr class=\"$css\"><td>${name//_/ }</td><td>$set_val</td><td>$mu_num</td><td>$ma_num</td><td class=\"st-$css\">$status</td></tr>"
}

{
cat <<'HTMLHEAD'
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>OpenEEBUS MPC Integration Test Report</title>
  <style>
    *, *::before, *::after { box-sizing: border-box; }
    body { font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif;
           background: #f4f6f8; margin: 0; padding: 2em; color: #2c3e50; }
    h1   { margin: 0 0 0.2em; font-size: 1.6em; }
    .meta { color: #7f8c8d; font-size: 0.9em; margin-bottom: 2em; }
    h2   { font-size: 1.1em; margin: 2em 0 0.6em; color: #34495e;
           border-left: 4px solid #3498db; padding-left: 0.6em; }
    table { border-collapse: collapse; width: 100%; max-width: 760px;
            background: #fff; border-radius: 6px;
            box-shadow: 0 1px 4px rgba(0,0,0,.12); }
    thead th { background: #2c3e50; color: #fff; padding: 9px 14px;
               text-align: left; font-weight: 600; font-size: 0.88em; }
    thead th:first-child  { border-radius: 6px 0 0 0; }
    thead th:last-child   { border-radius: 0 6px 0 0; }
    tbody td { padding: 7px 14px; font-size: 0.9em; border-bottom: 1px solid #ecf0f1; }
    tbody tr:last-child td { border-bottom: none; }
    tr.pass td { background: #f0faf4; }
    tr.fail td { background: #fdf2f2; }
    tr.na   td { background: #fefdf0; }
    .st-pass { color: #27ae60; font-weight: 700; }
    .st-fail { color: #e74c3c; font-weight: 700; }
    .st-na   { color: #e67e22; font-weight: 700; }
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

echo "  <h1>OpenEEBUS MPC Integration Test Report</h1>"
echo "  <div class=\"meta\">Generated: $TIMESTAMP &nbsp;|&nbsp; heat_pump (MU) + HEMS (MA) &nbsp;|&nbsp; MPC Scenarios 1–5</div>"

echo "  <h2>Scenario 1 — Momentary Active Power</h2>"
echo "  <table>"
echo "    <thead><tr><th>Measurement</th><th>MU set</th><th>MU get</th><th>MA get</th><th>Status</th></tr></thead>"
echo "    <tbody>"
html_mpc_row power_total   100
html_mpc_row power_phase_a 110
html_mpc_row power_phase_b 120
html_mpc_row power_phase_c 130
echo "    </tbody>"
echo "  </table>"

echo "  <h2>Scenario 2 — Energy Consumed / Produced</h2>"
echo "  <table>"
echo "    <thead><tr><th>Measurement</th><th>MU set</th><th>MU get</th><th>MA get</th><th>Status</th></tr></thead>"
echo "    <tbody>"
html_mpc_row energy_consumed 200
html_mpc_row energy_produced 210
echo "    </tbody>"
echo "  </table>"

echo "  <h2>Scenario 3 — Current per Phase</h2>"
echo "  <table>"
echo "    <thead><tr><th>Measurement</th><th>MU set</th><th>MU get</th><th>MA get</th><th>Status</th></tr></thead>"
echo "    <tbody>"
html_mpc_row current_phase_a 15
html_mpc_row current_phase_b 16
html_mpc_row current_phase_c 17
echo "    </tbody>"
echo "  </table>"

echo "  <h2>Scenario 4 — Voltage per Phase</h2>"
echo "  <table>"
echo "    <thead><tr><th>Measurement</th><th>MU set</th><th>MU get</th><th>MA get</th><th>Status</th></tr></thead>"
echo "    <tbody>"
html_mpc_row voltage_phase_a  230
html_mpc_row voltage_phase_b  231
html_mpc_row voltage_phase_c  232
html_mpc_row voltage_phase_ab 400
html_mpc_row voltage_phase_bc 401
html_mpc_row voltage_phase_ac 402
echo "    </tbody>"
echo "  </table>"

echo "  <h2>Scenario 5 — Frequency</h2>"
echo "  <table>"
echo "    <thead><tr><th>Measurement</th><th>MU set</th><th>MU get</th><th>MA get</th><th>Status</th></tr></thead>"
echo "    <tbody>"
html_mpc_row frequency 50
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
