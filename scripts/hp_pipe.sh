#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

# OS detection: add .exe suffix on Windows (Git Bash / MSYS2 / Cygwin)
case "$(uname -s)" in
  MINGW*|CYGWIN*|MSYS*) EXE=".exe" ;;
  *) EXE="" ;;
esac

# stdbuf is part of GNU coreutils; available on Linux, macOS (brew), and Git Bash
STDBUF=()
command -v stdbuf >/dev/null 2>&1 && STDBUF=(stdbuf -oL)

# Credentials — override via environment variables for other machines / OSes
HP_SKI="${EEBUS_HP_SKI:-1bb991d59a94cc1925486be3addb07200b9d7680}"
HP_CERT="${EEBUS_HP_CERT:-$SCRIPT_DIR/certificates/heat_pump.crt}"
HP_KEY="${EEBUS_HP_KEY:-$SCRIPT_DIR/certificates/heat_pump.key}"

HP_PIPE=/tmp/hp_pipe
HP_LOG=/tmp/hp.log

rm -f "$HP_PIPE"
mkfifo "$HP_PIPE"
: > "$HP_LOG"

# Keep write end open so heat_pump stdin never gets EOF
tail -f /dev/null > "$HP_PIPE" &
echo $! > /tmp/hp_keeper.pid

"${STDBUF[@]}" "$SCRIPT_DIR/../build/heat_pump$EXE" 4712 "$HP_SKI" "$HP_CERT" "$HP_KEY" \
    auto < "$HP_PIPE" >> "$HP_LOG" 2>&1 &

echo $! > /tmp/hp.pid
echo "heat_pump started (PID=$(cat /tmp/hp.pid)), pipe=$HP_PIPE, log=$HP_LOG"
