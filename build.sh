#!/usr/bin/env bash
#
# Copyright (C) 2026 LUMina Team
# Automated Builder Script for LUMina Tweaks
#

set -e

# ─── Environment Setup ─────────────────────────────────────────

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
VERSION="v1.1.0"
ZIP_NAME="LUMina-${VERSION}.zip"
BUILD_DIR="${PROJECT_DIR}/build_stage"
OUTPUT_DIR="${PROJECT_DIR}/out"

echo "========================================================="
echo "          LUMina Tweaks Automated Builder                "
echo "                      ${VERSION}                         "
echo "========================================================="

# ─── 1. Dependency Checks ──────────────────────────────────────

echo "[1/5] Checking build environment..."

if ! command -v g++ >/dev/null 2>&1; then
    echo "ERROR: g++ compiler is missing! Run: pkg install build-essential"
    exit 1
fi

if ! command -v zip >/dev/null 2>&1; then
    echo "Installing missing dependency: zip..."
    pkg install zip -y >/dev/null 2>&1
fi

# ─── 2. Compile C++ Core Engine (Single Monolith) ─────────────

echo "[2/5] Compiling C++ Native Engine (luminad)..."
cd "$PROJECT_DIR"

rm -f luminad

# Kompilasi murni 1 file main.cpp
g++ -O3 -std=c++17 main.cpp -o luminad

if [ ! -f "luminad" ]; then
    echo "CRITICAL ERROR: C++ Compilation failed!"
    exit 1
fi

echo "  -> Biner luminad berhasil dikompilasi dari main.cpp."

# ─── 3. Build WebUI Component ─────────────────────────────────

echo "[3/5] Processing WebUI frontend..."
if [ -d "${PROJECT_DIR}/webui" ]; then
    if [ -f "${PROJECT_DIR}/webui/build.sh" ]; then
        echo "  -> Executing webui/build.sh..."
        (cd "${PROJECT_DIR}/webui" && bash build.sh)
    elif command -v npm >/dev/null 2>&1 && [ -f "${PROJECT_DIR}/webui/package.json" ]; then
        echo "  -> Compiling WebUI via npm..."
        (cd "${PROJECT_DIR}/webui" && npm run build >/dev/null 2>&1)
    else
        echo "  -> Using pre-existing WebUI assets."
    fi
fi

# ─── 4. Assemble Module Staging Area ─────────────────────────

echo "[4/5] Assembling flashable module structure..."

rm -rf "$BUILD_DIR" "$OUTPUT_DIR"
mkdir -p "$BUILD_DIR"
mkdir -p "$OUTPUT_DIR"

if [ -d "${PROJECT_DIR}/module" ]; then
    cp -af "${PROJECT_DIR}/module/"* "$BUILD_DIR/"
else
    echo "CRITICAL ERROR: Folder 'module/' tidak ditemukan!"
    exit 1
fi

# Salin biner luminad
mkdir -p "${BUILD_DIR}/system/bin"
cp -f "${PROJECT_DIR}/luminad" "${BUILD_DIR}/luminad"
cp -f "${PROJECT_DIR}/luminad" "${BUILD_DIR}/system/bin/luminad"
chmod 755 "${BUILD_DIR}/luminad"
chmod 755 "${BUILD_DIR}/system/bin/luminad"

# Konversi format baris Windows (CRLF ke LF) & set izin eksekusi script .sh
find "$BUILD_DIR" -type f -name "*.sh" -exec sed -i 's/\r$//' {} +
find "$BUILD_DIR" -type f -name "*.sh" -exec chmod 755 {} +

if [ -d "${PROJECT_DIR}/webui/dist" ]; then
    mkdir -p "${BUILD_DIR}/webroot"
    cp -af "${PROJECT_DIR}/webui/dist/"* "${BUILD_DIR}/webroot/"
fi

if [ ! -d "${BUILD_DIR}/META-INF/com/google/android" ]; then
    mkdir -p "${BUILD_DIR}/META-INF/com/google/android"
    cat <<'EOF' > "${BUILD_DIR}/META-INF/com/google/android/update-binary"
#!/sbin/sh
umask 022
echo "Installing LUMina Tweaks..."
EOF
    cat <<'EOF' > "${BUILD_DIR}/META-INF/com/google/android/updater-script"
# MAGISK
EOF
fi

# ─── 5. Package Flashable ZIP ─────────────────────────────────

echo "[5/5] Packaging into ${ZIP_NAME}..."
cd "$BUILD_DIR"
zip -r9 -q "${OUTPUT_DIR}/${ZIP_NAME}" ./*

rm -rf "$BUILD_DIR"

echo "========================================================="
echo " SUCCESS: Module built and packaged successfully"
echo " File : ${OUTPUT_DIR}/${ZIP_NAME}"
echo " Size : $(du -h "${OUTPUT_DIR}/${ZIP_NAME}" | cut -f1)"
echo "========================================================="
