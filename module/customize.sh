#!/system/bin/sh
#
# Copyright (C) 2026 LUMina Team
#

SKIPUNZIP=0

MODULE_CONFIG="/data/adb/.config/lumina"
BIN_DIR="$MODPATH/system/bin"
DAEMON_BIN="$BIN_DIR/luminad"

# ─── Helper Functions ─────────────────────────────────────────

abort_unsupported_arch() {
	ui_print "*********************************************************"
	ui_print "! Arsitektur tidak didukung: $ARCH"
	ui_print "! LUMina Tweaks membutuhkan 64-bit (arm64-v8a)."
	abort "*********************************************************"
}

abort_android_version() {
	ui_print "*********************************************************"
	ui_print "! Versi Android tidak didukung: API $API"
	ui_print "! LUMina Tweaks membutuhkan minimal Android 10 (API 29)."
	abort "*********************************************************"
}

# ─── 1. Pre-Installation Checks ───────────────────────────────

[ "$API" -lt 29 ] && abort_android_version

case "$ARCH" in
    "arm64") ARCH_TMP="arm64-v8a" ;;
    *) abort_unsupported_arch ;;
esac

ui_print "---------------------------------------------------------"
ui_print "             LUMina Tweaks Engine v1.1                   "
ui_print "                Modular C++ Backend                      "
ui_print "---------------------------------------------------------"

# ─── 2. Extract Core Binary ───────────────────────────────────

ui_print "- Mengekstrak binary C++ ($ARCH_TMP)..."
mkdir -p "$BIN_DIR"

if [ -d "$TMPDIR/libs/$ARCH_TMP" ]; then
    cp -rf "$TMPDIR/libs/$ARCH_TMP/"* "$BIN_DIR/" 2>/dev/null
    rm -rf "$TMPDIR/libs"
elif [ -f "$TMPDIR/luminad" ]; then
    cp -f "$TMPDIR/luminad" "$DAEMON_BIN"
fi

if [ ! -f "$DAEMON_BIN" ]; then
    ui_print "! Critical Error: binary luminad tidak ditemukan di zip!"
    abort "---------------------------------------------------------"
fi

# ─── 3. Environment Flags ─────────────────────────────────────

touch "$MODPATH/skip_mountify"

# ─── 4. KernelSU / APatch Integration ─────────────────────────

if [ "$KSU" = "true" ] || [ "$APATCH" = "true" ]; then
    ui_print "- KernelSU / APatch terdeteksi"
    touch "$MODPATH/skip_mount"

    manager_paths="/data/adb/ap/bin /data/adb/ksu/bin"
    for dir in $manager_paths; do
        if [ -d "$dir" ]; then
            ln -sf "$DAEMON_BIN" "$dir/luminad"
        fi
    done
fi

# ─── 5. Deploy WebUI ──────────────────────────────────────────

if unzip -l "$ZIPFILE" "webroot/*" >/dev/null 2>&1; then
    ui_print "- Memasang WebUI..."
    unzip -o "$ZIPFILE" "webroot/*" -d "$MODPATH" >/dev/null 2>&1
fi

# ─── 6. Inisialisasi Config (Sinkron dengan C++ Backend) ───────

ui_print "- Menyiapkan folder konfigurasi..."
mkdir -p "$MODULE_CONFIG"

# Default config.json (hanya parameter aktif backend)
if [ ! -f "$MODULE_CONFIG/config.json" ]; then
    ui_print "- Membuat default config.json..."
    cat <<'EOF' > "$MODULE_CONFIG/config.json"
{
  "lite_mode": false,
  "disable_thermal": false,
  "dnd_mode": false
}
EOF
fi

# Default gamelist.json
if [ ! -f "$MODULE_CONFIG/gamelist.json" ]; then
    ui_print "- Membuat default gamelist.json..."
    cat <<'EOF' > "$MODULE_CONFIG/gamelist.json"
{
  "com.mobile.legends": {},
  "com.dts.freefireth": {}
}
EOF
fi

# ─── 7. Set Permissions ───────────────────────────────────────

ui_print "- Mengatur permission executable..."
set_perm_recursive "$BIN_DIR" 0 0 0755 0755
chmod 755 "$DAEMON_BIN"

ui_print "---------------------------------------------------------"
ui_print "         LUMina Tweaks Berhasil Dipasang!                "
ui_print "---------------------------------------------------------"
