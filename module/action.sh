#!/system/bin/sh
#
# Copyright (C) 2026 LUMina Team
#

MODULE_DIR="/data/adb/modules/lumina"
DAEMON_BIN="$MODULE_DIR/system/bin/luminad"


[ ! -f "$DAEMON_BIN" ] && DAEMON_BIN="$MODULE_DIR/luminad"
[ ! -f "$DAEMON_BIN" ] && DAEMON_BIN="/system/bin/luminad"

DAEMON_PID=$(pgrep -f "luminad")


if [ -z "$DAEMON_PID" ] && [ -f "$DAEMON_BIN" ]; then
    nohup "$DAEMON_BIN" daemon >/dev/null 2>&1 &
    sleep 1
    DAEMON_PID=$(pgrep -f "luminad")
    if [ -n "$DAEMON_PID" ]; then
        echo "[LUMina] Daemon dihidupkan ulang (PID: $DAEMON_PID)"
        exit 0
    else
        echo "[LUMina] Gagal memulai daemon!"
        exit 1
    fi
fi

if [ -n "$DAEMON_PID" ]; then
    echo "[LUMina] Daemon aktif (PID: $DAEMON_PID)"
else
    echo "[LUMina] Daemon mati"
fi
