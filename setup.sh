#!/bin/bash

# Setup script for Brick Game Engine
# Instala SDL2 y compila el proyecto

echo "================================"
echo "Brick Game Engine - Setup"
echo "================================"
echo ""

# Detect OS
if [[ "$OSTYPE" == "darwin"* ]]; then
    echo "Detected macOS"
    echo "Installing SDL2 with Homebrew..."
    
    if ! command -v brew &> /dev/null; then
        echo "Error: Homebrew not found. Please install Homebrew first."
        echo "Visit: https://brew.sh"
        exit 1
    fi
    
    brew install sdl2
    
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo "Detected Linux"
    echo "Installing SDL2 development files..."
    
    if command -v apt-get &> /dev/null; then
        sudo apt-get update
        sudo apt-get install -y libsdl2-dev cmake build-essential
    elif command -v yum &> /dev/null; then
        sudo yum install -y SDL2-devel cmake gcc-c++
    else
        echo "Unsupported package manager"
        exit 1
    fi
    
else
    echo "Unsupported OS. Please install SDL2 manually."
    exit 1
fi

echo ""
echo "Installing CMake and build tools..."

if [[ "$OSTYPE" == "darwin"* ]]; then
    brew install cmake
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    if command -v apt-get &> /dev/null; then
        sudo apt-get install -y cmake build-essential
    fi
fi

echo ""
echo "Building Brick Game Engine..."
echo ""

cd "$(dirname "$0")/brickc/build"

if [ ! -d "." ]; then
    mkdir -p build
    cd build
fi

echo "Running CMake..."
cmake ..

echo ""
echo "Compiling..."
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu)

if [ $? -eq 0 ]; then
    echo ""
    echo "================================"
    echo "✅ Build successful!"
    echo "================================"
    echo ""
    echo "To run the Snake game:"
    echo "  ./bin/brick_snake"
    echo ""
    echo "Enjoy the game! 🐍"
else
    echo ""
    echo "================================"
    echo "❌ Build failed!"
    echo "================================"
    exit 1
fi

