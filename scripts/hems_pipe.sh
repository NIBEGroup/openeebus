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
HEMS_SKI="${EEBUS_HEMS_SKI:-40c61c3526f271e8e1547851c46f6ea20d4c6f83}"
HEMS_CERT="${EEBUS_HEMS_CERT:-$SCRIPT_DIR/certificates/hems.crt}"
HEMS_KEY="${EEBUS_HEMS_KEY:-$SCRIPT_DIR/certificates/hems.key}"

HEMS_PIPE=/tmp/hems_pipe
HEMS_LOG=/tmp/hems.log

rm -f "$HEMS_PIPE"
mkfifo "$HEMS_PIPE"
: > "$HEMS_LOG"

# Keep write end open so hems stdin never gets EOF
tail -f /dev/null > "$HEMS_PIPE" &
echo $! > /tmp/hems_keeper.pid

"${STDBUF[@]}" "$SCRIPT_DIR/../build/hems$EXE" 4710 "$HEMS_SKI" "$HEMS_CERT" "$HEMS_KEY" \
    auto < "$HEMS_PIPE" >> "$HEMS_LOG" 2>&1 &

echo $! > /tmp/hems.pid
echo "hems started (PID=$(cat /tmp/hems.pid)), pipe=$HEMS_PIPE, log=$HEMS_LOG"
