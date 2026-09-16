#!/system/bin/sh
# Copyright (C) 2026 LUMina Team

MODDIR="${0%/*}"
[ -z "$MODDIR" ] || [ "$MODDIR" = "." ] && MODDIR="/data/adb/modules/lumina"

export PATH="/system/bin:/system/xbin:/data/adb/ap/bin:/data/adb/ksu/bin:$PATH"

while [ "$(getprop sys.boot_completed)" != "1" ]; do
    sleep 3
done
sleep 5

CONFIG_DIR="/data/adb/.config/lumina"
mkdir -p "$CONFIG_DIR"

BIN="$MODDIR/system/bin/luminad"
[ -f "$BIN" ] || BIN="/data/adb/modules/lumina/system/bin/luminad"

[ ! -f "$BIN" ] && exit 1

chmod 755 "$BIN"
chcon u:object_r:system_file:s0 "$BIN" 2>/dev/null
killall -9 luminad 2>/dev/null

while true; do
    if ! pidof luminad >/dev/null 2>&1; then
        "$BIN" daemon > "$CONFIG_DIR/daemon.log" 2>&1 &
    fi
    sleep 5
done
