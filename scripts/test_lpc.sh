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

echo "Waiting for EG LPC remote entity to be connected (up to 30s)..."
for i in $(seq 1 30); do
  if grep -q "EG LPC Failsafe Active Power Limit received" "$HEMS_LOG" 2>/dev/null; then
    echo "EG LPC entity ready after ${i}s"; break
  fi
  sleep 1
done

# -----------------------------------------------------------------------
# LPC Scenario 1: CS (heat_pump) announces failsafe parameters → EG (HEMS)
# -----------------------------------------------------------------------
echo "=== [LPC Scenario 1] CS sets failsafe parameters ==="
echo "cs_lpc set failsafe_limit 5000 true"    > "$HP_PIPE"; sleep 0.3
echo "cs_lpc set failsafe_duration PT2H true" > "$HP_PIPE"; sleep 0.3

echo "Waiting 3s for failsafe parameters to propagate to HEMS..."
sleep 3

echo "cs_lpc get failsafe_limit"    > "$HP_PIPE";   sleep 0.2
echo "cs_lpc get failsafe_duration" > "$HP_PIPE";   sleep 0.2
echo "eg_lpc get failsafe_limit"    > "$HEMS_PIPE"; sleep 0.2
echo "eg_lpc get failsafe_duration" > "$HEMS_PIPE"; sleep 0.2
sleep 1

s1_cs_limit_raw=$(grep "CS LPC Failsafe Active Power Limit" "$HP_LOG"   | tail -1 | grep -o 'value=[^ ,]*' | head -1)
s1_eg_limit_raw=$(grep "EG LPC Failsafe Active Power Limit" "$HEMS_LOG" | tail -1 | grep -o 'value=[^ ,]*' | head -1)
s1_cs_dur_raw=$(  grep "CS LPC Failsafe Duration Minimum"   "$HP_LOG"   | tail -1 | grep -oE 'PT[^ ,]*'    | head -1)
s1_eg_dur_raw=$(  grep "EG LPC Failsafe Duration Minimum"   "$HEMS_LOG" | tail -1 | grep -oE 'PT[^ ,]*'    | head -1)

# -----------------------------------------------------------------------
# LPC Scenario 2: EG (HEMS) writes active power limit → CS (heat_pump)
# -----------------------------------------------------------------------
echo "=== [LPC Scenario 2] EG writes active power limit to CS ==="
echo "eg_lpc set power_limit 7000 PT0S true" > "$HEMS_PIPE"; sleep 0.3

echo "Waiting 3s for active limit to propagate to heat_pump..."
sleep 3

echo "cs_lpc get power_limit" > "$HP_PIPE";   sleep 0.2
echo "eg_lpc get power_limit" > "$HEMS_PIPE"; sleep 0.2
sleep 1

s2_cs_val_raw=$(grep "CS LPC Active Power Limit" "$HP_LOG"   | grep -v "Failsafe" | tail -1 | grep -o 'value=[^ ,]*'    | head -1)
s2_eg_val_raw=$(grep "EG LPC Active Power Limit" "$HEMS_LOG" | grep -v "Failsafe" | tail -1 | grep -o 'value=[^ ,]*'    | head -1)
s2_cs_dur_raw=$(grep "CS LPC Active Power Limit" "$HP_LOG"   | grep -v "Failsafe" | tail -1 | grep -o 'duration=[^ ,]*' | head -1)
s2_eg_dur_raw=$(grep "EG LPC Active Power Limit" "$HEMS_LOG" | grep -v "Failsafe" | tail -1 | grep -o 'duration=[^ ,]*' | head -1)
s2_cs_act_raw=$(grep "CS LPC Active Power Limit" "$HP_LOG"   | grep -v "Failsafe" | tail -1 | grep -o 'is active=[^ ]*' | head -1)
s2_eg_act_raw=$(grep "EG LPC Active Power Limit" "$HEMS_LOG" | grep -v "Failsafe" | tail -1 | grep -o 'active=[^ ]*'   | head -1)

# -----------------------------------------------------------------------
# LPC Scenario 3: EG (HEMS) writes failsafe parameters → CS (heat_pump)
# -----------------------------------------------------------------------
echo "=== [LPC Scenario 3] EG writes failsafe parameters to CS ==="
echo "eg_lpc set failsafe_limit 600"     > "$HEMS_PIPE"; sleep 0.3
echo "eg_lpc set failsafe_duration PT3H" > "$HEMS_PIPE"; sleep 0.3

echo "Waiting 3s for failsafe parameters to propagate to heat_pump..."
sleep 3

echo "cs_lpc get failsafe_limit"    > "$HP_PIPE";   sleep 0.2
echo "cs_lpc get failsafe_duration" > "$HP_PIPE";   sleep 0.2
echo "eg_lpc get failsafe_limit"    > "$HEMS_PIPE"; sleep 0.2
echo "eg_lpc get failsafe_duration" > "$HEMS_PIPE"; sleep 0.2
sleep 1

s3_cs_limit_raw=$(grep "CS LPC Failsafe Active Power Limit" "$HP_LOG"   | tail -1 | grep -o 'value=[^ ,]*' | head -1)
s3_eg_limit_raw=$(grep "EG LPC Failsafe Active Power Limit" "$HEMS_LOG" | tail -1 | grep -o 'value=[^ ,]*' | head -1)
s3_cs_dur_raw=$(  grep "CS LPC Failsafe Duration Minimum"   "$HP_LOG"   | tail -1 | grep -oE 'PT[^ ,]*'    | head -1)
s3_eg_dur_raw=$(  grep "EG LPC Failsafe Duration Minimum"   "$HEMS_LOG" | tail -1 | grep -oE 'PT[^ ,]*'    | head -1)

# -----------------------------------------------------------------------
# LPC Scenario 4: CS (heat_pump) announces consumption nominal max → EG (HEMS)
# -----------------------------------------------------------------------
echo "=== [LPC Scenario 4] CS sets consumption nominal max ==="
echo "cs_lpc set nominal_max 11000" > "$HP_PIPE"; sleep 0.3

echo "Waiting 3s for nominal max to propagate to HEMS..."
sleep 3

echo "cs_lpc get nominal_max"       > "$HP_PIPE";   sleep 0.2
echo "eg_lpc get power_nominal_max" > "$HEMS_PIPE"; sleep 0.2
sleep 1

s4_cs_raw=$(grep "CS LPC Nominal Max"       "$HP_LOG"   | tail -1 | grep -o 'value=[^ ,]*' | head -1)
s4_eg_raw=$(grep "EG LPC Power Nominal Max" "$HEMS_LOG" | tail -1 | grep -o 'value=[^ ,]*' | head -1)

# -----------------------------------------------------------------------
# LPC Scenario 5: CS (heat_pump) announces its own active power limit → EG (HEMS)
# -----------------------------------------------------------------------
echo "=== [LPC Scenario 5] CS announces active power limit ==="
echo "cs_lpc set power_limit 9000 true true" > "$HP_PIPE"; sleep 0.3

echo "Waiting 3s for active limit to propagate to HEMS..."
sleep 3

echo "cs_lpc get power_limit" > "$HP_PIPE";   sleep 0.2
echo "eg_lpc get power_limit" > "$HEMS_PIPE"; sleep 0.2
sleep 1

s5_cs_val_raw=$(grep "CS LPC Active Power Limit" "$HP_LOG"   | grep -v "Failsafe" | tail -1 | grep -o 'value=[^ ,]*'    | head -1)
s5_eg_val_raw=$(grep "EG LPC Active Power Limit" "$HEMS_LOG" | grep -v "Failsafe" | tail -1 | grep -o 'value=[^ ,]*'    | head -1)
s5_cs_act_raw=$(grep "CS LPC Active Power Limit" "$HP_LOG"   | grep -v "Failsafe" | tail -1 | grep -o 'is active=[^ ]*' | head -1)
s5_eg_act_raw=$(grep "EG LPC Active Power Limit" "$HEMS_LOG" | grep -v "Failsafe" | tail -1 | grep -o 'active=[^ ]*'   | head -1)

# -----------------------------------------------------------------------
# Graceful shutdown
# -----------------------------------------------------------------------
echo "Sending exit to both nodes..."
echo "exit" > "$HP_PIPE"   || true
echo "exit" > "$HEMS_PIPE" || true
sleep 1
kill "$(cat /tmp/hp_keeper.pid)" "$(cat /tmp/hems_keeper.pid)" 2>/dev/null || true

# -----------------------------------------------------------------------
# Print text results
# -----------------------------------------------------------------------
echo ""
echo "======================================================================"
echo "  LPC Scenario 1 — Failsafe (CS heat_pump announces → EG HEMS reads)"
echo "======================================================================"
printf "%-26s  %16s  %16s  %16s\n" "Parameter" "CS set" "CS get" "EG get"
printf "%-26s  %16s  %16s  %16s\n" \
  "--------------------------" "----------------" "----------------" "----------------"
printf "%-26s  %16s  %16s  %16s\n" \
  "failsafe power limit (W)" "value=5000" "${s1_cs_limit_raw:-N/A}" "${s1_eg_limit_raw:-N/A}"
printf "%-26s  %16s  %16s  %16s\n" \
  "failsafe duration" "PT2H" "${s1_cs_dur_raw:-N/A}" "${s1_eg_dur_raw:-N/A}"

echo ""
echo "======================================================================"
echo "  LPC Scenario 2 — Active Power Limit (EG HEMS writes → CS heat_pump receives)"
echo "======================================================================"
printf "%-26s  %16s  %16s  %16s\n" "Parameter" "EG set" "CS get" "EG get"
printf "%-26s  %16s  %16s  %16s\n" \
  "--------------------------" "----------------" "----------------" "----------------"
printf "%-26s  %16s  %16s  %16s\n" \
  "active power limit (W)" "value=7000" "${s2_cs_val_raw:-N/A}" "${s2_eg_val_raw:-N/A}"
printf "%-26s  %16s  %16s  %16s\n" \
  "active limit duration" "duration=PT0S" "${s2_cs_dur_raw:-N/A}" "${s2_eg_dur_raw:-N/A}"
printf "%-26s  %16s  %16s  %16s\n" \
  "is active" "is active=true" "${s2_cs_act_raw:-N/A}" "${s2_eg_act_raw:-N/A}"

echo ""
echo "======================================================================"
echo "  LPC Scenario 3 — Failsafe (EG HEMS writes → CS heat_pump stores)"
echo "======================================================================"
printf "%-26s  %16s  %16s  %16s\n" "Parameter" "EG set" "CS get" "EG get"
printf "%-26s  %16s  %16s  %16s\n" \
  "--------------------------" "----------------" "----------------" "----------------"
printf "%-26s  %16s  %16s  %16s\n" \
  "failsafe power limit (W)" "value=600" "${s3_cs_limit_raw:-N/A}" "${s3_eg_limit_raw:-N/A}"
printf "%-26s  %16s  %16s  %16s\n" \
  "failsafe duration" "PT3H" "${s3_cs_dur_raw:-N/A}" "${s3_eg_dur_raw:-N/A}"

echo ""
echo "======================================================================"
echo "  LPC Scenario 4 — Nominal Max (CS heat_pump announces → EG HEMS reads)"
echo "======================================================================"
printf "%-32s  %14s  %14s  %14s\n" "Parameter" "CS set" "CS get" "EG get"
printf "%-32s  %14s  %14s  %14s\n" \
  "--------------------------------" "--------------" "--------------" "--------------"
printf "%-32s  %14s  %14s  %14s\n" \
  "consumption nominal max (W)" "value=11000" "${s4_cs_raw:-N/A}" "${s4_eg_raw:-N/A}"

echo ""
echo "======================================================================"
echo "  LPC Scenario 5 — Active Power Limit (CS heat_pump announces → EG HEMS reads)"
echo "======================================================================"
printf "%-26s  %16s  %16s  %16s\n" "Parameter" "CS set" "CS get" "EG get"
printf "%-26s  %16s  %16s  %16s\n" \
  "--------------------------" "----------------" "----------------" "----------------"
printf "%-26s  %16s  %16s  %16s\n" \
  "active power limit (W)" "value=9000" "${s5_cs_val_raw:-N/A}" "${s5_eg_val_raw:-N/A}"
printf "%-26s  %16s  %16s  %16s\n" \
  "is active" "is active=true" "${s5_cs_act_raw:-N/A}" "${s5_eg_act_raw:-N/A}"

echo ""
echo "To force-stop any remaining processes: kill \$(cat /tmp/hp.pid) \$(cat /tmp/hems.pid) \$(cat /tmp/hp_keeper.pid) \$(cat /tmp/hems_keeper.pid) 2>/dev/null || true"

# -----------------------------------------------------------------------
# HTML report
# -----------------------------------------------------------------------
HTML_REPORT=/tmp/eebus_lpc_test_report.html
TIMESTAMP=$(date "+%Y-%m-%d %H:%M:%S")
PASS_COUNT=0; FAIL_COUNT=0; NA_COUNT=0

html_row() {
  local name=$1 set_val=$2 cs_val=$3 eg_val=$4 status css
  if   [ "$cs_val" = "N/A" ] || [ "$eg_val" = "N/A" ]; then
    status="N/A"; css="na"; NA_COUNT=$((NA_COUNT+1))
  elif [ "$cs_val" = "$set_val" ] && [ "$eg_val" = "$set_val" ]; then
    status="PASS"; css="pass"; PASS_COUNT=$((PASS_COUNT+1))
  else
    status="FAIL"; css="fail"; FAIL_COUNT=$((FAIL_COUNT+1))
  fi
  echo "      <tr class=\"$css\"><td>${name//_/ }</td><td>${set_val#value=}</td><td>$cs_val</td><td>$eg_val</td><td class=\"st-$css\">$status</td></tr>"
}

html_dur_row() {
  local name=$1 set_val=$2 cs_val=$3 eg_val=$4 status css
  if   [ "$cs_val" = "N/A" ] || [ "$eg_val" = "N/A" ]; then
    status="N/A"; css="na"; NA_COUNT=$((NA_COUNT+1))
  elif [ "$cs_val" = "$set_val" ] && [ "$eg_val" = "$set_val" ]; then
    status="PASS"; css="pass"; PASS_COUNT=$((PASS_COUNT+1))
  else
    status="FAIL"; css="fail"; FAIL_COUNT=$((FAIL_COUNT+1))
  fi
  echo "      <tr class=\"$css\"><td>${name//_/ }</td><td>$set_val</td><td>$cs_val</td><td>$eg_val</td><td class=\"st-$css\">$status</td></tr>"
}

html_bool_row() {
  local name=$1 set_bool=$2 cs_raw=$3 eg_raw=$4
  local cs_val eg_val status css
  cs_val="${cs_raw##*=}"; [ -z "$cs_val" ] && cs_val="N/A"
  eg_val="${eg_raw##*=}"; [ -z "$eg_val" ] && eg_val="N/A"
  if   [ "$cs_val" = "N/A" ] || [ "$eg_val" = "N/A" ]; then
    status="N/A"; css="na"; NA_COUNT=$((NA_COUNT+1))
  elif [ "$cs_val" = "$set_bool" ] && [ "$eg_val" = "$set_bool" ]; then
    status="PASS"; css="pass"; PASS_COUNT=$((PASS_COUNT+1))
  else
    status="FAIL"; css="fail"; FAIL_COUNT=$((FAIL_COUNT+1))
  fi
  echo "      <tr class=\"$css\"><td>${name//_/ }</td><td>$set_bool</td><td>$cs_val</td><td>$eg_val</td><td class=\"st-$css\">$status</td></tr>"
}

{
cat <<'HTMLHEAD'
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>OpenEEBUS LPC Integration Test Report</title>
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

echo "  <h1>OpenEEBUS LPC Integration Test Report</h1>"
echo "  <div class=\"meta\">Generated: $TIMESTAMP &nbsp;|&nbsp; heat_pump (CS) + HEMS (EG) &nbsp;|&nbsp; LPC Scenarios 1–5</div>"

echo "  <h2>Scenario 1 — Failsafe Parameters (CS heat_pump announces → EG HEMS reads)</h2>"
echo "  <table>"
echo "    <thead><tr><th>Parameter</th><th>CS set</th><th>CS get</th><th>EG get</th><th>Status</th></tr></thead>"
echo "    <tbody>"
html_row     "failsafe power limit (W)" "value=5000" "${s1_cs_limit_raw:-N/A}" "${s1_eg_limit_raw:-N/A}"
html_dur_row "failsafe duration"        "PT2H"        "${s1_cs_dur_raw:-N/A}"   "${s1_eg_dur_raw:-N/A}"
echo "    </tbody>"
echo "  </table>"

echo "  <h2>Scenario 2 — Active Power Limit (EG HEMS writes → CS heat_pump receives)</h2>"
echo "  <table>"
echo "    <thead><tr><th>Parameter</th><th>EG set</th><th>CS get</th><th>EG get</th><th>Status</th></tr></thead>"
echo "    <tbody>"
html_row      "active power limit (W)" "value=7000"    "${s2_cs_val_raw:-N/A}" "${s2_eg_val_raw:-N/A}"
html_dur_row  "active limit duration"  "duration=PT0S" "${s2_cs_dur_raw:-N/A}" "${s2_eg_dur_row:-N/A}"
html_bool_row "is active"              "true"          "${s2_cs_act_raw:-}"    "${s2_eg_act_raw:-}"
echo "    </tbody>"
echo "  </table>"

echo "  <h2>Scenario 3 — Failsafe Parameters (EG HEMS writes → CS heat_pump stores)</h2>"
echo "  <table>"
echo "    <thead><tr><th>Parameter</th><th>EG set</th><th>CS get</th><th>EG get</th><th>Status</th></tr></thead>"
echo "    <tbody>"
html_row     "failsafe power limit (W)" "value=600" "${s3_cs_limit_raw:-N/A}" "${s3_eg_limit_raw:-N/A}"
html_dur_row "failsafe duration"        "PT3H"       "${s3_cs_dur_raw:-N/A}"   "${s3_eg_dur_raw:-N/A}"
echo "    </tbody>"
echo "  </table>"

echo "  <h2>Scenario 4 — Nominal Max (CS heat_pump announces → EG HEMS reads)</h2>"
echo "  <table>"
echo "    <thead><tr><th>Parameter</th><th>CS set</th><th>CS get</th><th>EG get</th><th>Status</th></tr></thead>"
echo "    <tbody>"
html_row "consumption nominal max (W)" "value=11000" "${s4_cs_raw:-N/A}" "${s4_eg_raw:-N/A}"
echo "    </tbody>"
echo "  </table>"

echo "  <h2>Scenario 5 — Active Power Limit (CS heat_pump announces → EG HEMS reads)</h2>"
echo "  <table>"
echo "    <thead><tr><th>Parameter</th><th>CS set</th><th>CS get</th><th>EG get</th><th>Status</th></tr></thead>"
echo "    <tbody>"
html_row      "active power limit (W)" "value=9000" "${s5_cs_val_raw:-N/A}" "${s5_eg_val_raw:-N/A}"
html_bool_row "is active"              "true"        "${s5_cs_act_raw:-}"    "${s5_eg_act_raw:-}"
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
