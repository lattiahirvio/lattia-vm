#!/bin/sh
set -e 

BUILD_DIR="./build/"
OUTPUT_BINARY="lattia-vm"

if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating build directory: $BUILD_DIR"
    mkdir -p "$BUILD_DIR"
fi
cd "$BUILD_DIR"
echo "Building lattia-vm..."

if [ ! -f "$OUTPUT_BINARY" ]; then
    echo "Error: Build failed, $OUTPUT_BINARY not found!"
    exit 1
fi

echo "Moving $OUTPUT_BINARY to project root..."
rm -f "../$OUTPUT_BINARY"
mv "$OUTPUT_BINARY" ..

echo "Build completed successfully!"
