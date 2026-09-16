#!/system/bin/sh
# Copyright (C) 2026 LUMina Team

killall -9 luminad 2>/dev/null
pkill -9 -f luminad 2>/dev/null

echo enabled > /sys/class/thermal/thermal_zone0/mode 2>/dev/null

start thermal 2>/dev/null
start thermald 2>/dev/null
start vendor.thermal-hal-2-0 2>/dev/null
start vendor.thermal-hal-1-0 2>/dev/null
start mi_thermald 2>/dev/null
start thermal_monitor 2>/dev/null

rm -rf /data/adb/.config/lumina
rm -f /data/adb/service.d/.lumina_cleanup.sh
rm -f /data/adb/ap/bin/luminad /data/adb/ksu/bin/luminad
