#!/bin/sh
set -e 

BUILD_DIR="./build"
OUTPUT_BINARY="lattia-vm"

if [ ! -f "CMakeLists.txt" ]; then
    echo "CMakeLists.txt not found in the current directory."
    exit 1
fi

if [ ! -d "$BUILD_DIR" ]; then
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    cmake ..
    echo "Makefile generated successfully."
else
    cd "$BUILD_DIR"
fi

echo "Building $OUTPUT_BINARY..."
make -j$(nproc)  # Uses multiple cores for faster builds

if [ ! -f "$OUTPUT_BINARY" ]; then
    echo "Error: Build failed, $OUTPUT_BINARY not found!"
    exit 1
fi

echo "Moving $OUTPUT_BINARY to project root..."
mv -f "$OUTPUT_BINARY" ..

echo "Build completed successfully!"

