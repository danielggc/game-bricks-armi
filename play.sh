#!/bin/bash

# Play Brick Games - Main Launcher
# Menú principal para seleccionar entre Snake y Tetris

echo "🎮 Brick Games - Main Menu"
echo "============================="
echo ""

# Check if the executable exists
if [ ! -f "runtime/build/bin/brick_games" ]; then
    echo "Error: Main launcher executable not found!"
    echo ""
    echo "Please compile the project first:"
    echo "  1. ./setup.sh"
    echo ""
    echo "Or manually:"
    echo "  cd runtime"
    echo "  mkdir -p build"
    echo "  cd build"
    echo "  cmake .."
    echo "  make"
    exit 1
fi

echo "Starting Brick Games Menu..."
echo "Select a game using arrow keys"
echo "Press SPACE or ENTER to select"
echo ""

cd "$(dirname "$0")"
./runtime/build/bin/brick_games

echo ""
echo "Thanks for playing! 🎮"

