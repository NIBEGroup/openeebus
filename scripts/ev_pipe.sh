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
EV_SKI="${EEBUS_EV_SKI:-1bb991d59a94cc1925486be3addb07200b9d7680}"
EV_CERT="${EEBUS_EV_CERT:-$SCRIPT_DIR/certificates/ev_charger.crt}"
EV_KEY="${EEBUS_EV_KEY:-$SCRIPT_DIR/certificates/ev_charger.key}"

EV_PIPE=/tmp/ev_pipe
EV_LOG=/tmp/ev.log

rm -f "$EV_PIPE"
mkfifo "$EV_PIPE"
: > "$EV_LOG"

# Keep write end open so ev_charger stdin never gets EOF
tail -f /dev/null > "$EV_PIPE" &
echo $! > /tmp/ev_keeper.pid

"${STDBUF[@]}" "$SCRIPT_DIR/../build/ev_charger$EXE" 4714 "$EV_SKI" "$EV_CERT" "$EV_KEY" \
    auto < "$EV_PIPE" >> "$EV_LOG" 2>&1 &

echo $! > /tmp/ev.pid
echo "ev_charger started (PID=$(cat /tmp/ev.pid)), pipe=$EV_PIPE, log=$EV_LOG"
