#!/bin/bash

# ALG Welcome - Build Script
# This script builds the Qt6/C++ version of ALG Welcome

set -e  # Exit on error

echo "================================"
echo "ALG Welcome - Build Script"
echo "================================"
echo

# Check if Qt6 is available
echo "Checking for Qt6..."
if ! command -v qmake6 &> /dev/null && ! command -v qmake &> /dev/null; then
    echo "Warning: Qt6 qmake not found in PATH"
    echo "Make sure Qt6 is installed and available"
fi

# Create build directory
BUILD_DIR="build"
if [ -d "$BUILD_DIR" ]; then
    echo "Build directory exists. Cleaning..."
    rm -rf "$BUILD_DIR"
fi

echo "Creating build directory..."
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Run CMake
echo
echo "Running CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
echo
echo "Building project..."
make -j$(nproc)

# Check if build was successful
if [ $? -eq 0 ]; then
    echo
    echo "================================"
    echo "Build completed successfully!"
    echo "================================"
    echo
    echo "To run the application:"
    echo "  cd $BUILD_DIR && ./alg-welcome"
    echo
    echo "To install system-wide:"
    echo "  cd $BUILD_DIR && sudo make install"
    echo
else
    echo
    echo "================================"
    echo "Build failed!"
    echo "================================"
    exit 1
fi
