#!/bin/bash
set -e

echo "========================================================="
echo "          Lumina Tweaks Automated Builder"
echo "                      v1.1.0"
echo "========================================================="

# 1. Cek dependensi compile
echo "[1/4] Checking build environment..."
if ! command -v clang++ &>/dev/null; then
    echo "[-] clang++ not found. Installing clang..."
    pkg install clang -y
fi

if ! command -v zip &>/dev/null; then
    echo "[-] zip not found. Installing zip..."
    pkg install zip -y
fi

# 2. Compile binary native C++
echo "[2/4] Compiling C++ Native Engine (luminad)..."
mkdir -p module/system/bin

clang++ -O3 -std=c++17 -pthread -Iinclude \
    src/utils.cpp \
    src/config.cpp \
    src/profile.cpp \
    src/hardware.cpp \
    src/monitor.cpp \
    src/main.cpp \
    -o module/system/bin/luminad

chmod 755 module/system/bin/luminad
strip module/system/bin/luminad 2>/dev/null || true

echo "[+] Native engine built successfully."

# 3. Validasi struktur module
echo "[3/4] Verifying module structure..."
if [ ! -d "module" ]; then
    echo "[-] Directory 'module' not found!"
    exit 1
fi

# 4. Packaging module ke format ZIP flashable
echo "[4/4] Packaging flashable zip..."
ZIP_NAME="LuminaTweaks-v1.1.0.zip"
rm -f "$ZIP_NAME"

cd module
zip -r9 "../$ZIP_NAME" ./* -x "*.bak" "*.tmp"
cd ..

echo "========================================================="
echo "[+] Build complete: $ZIP_NAME"
echo "========================================================="
