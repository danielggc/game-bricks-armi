#!/bin/bash

# Play Brick Tetris Game
# Script de conveniencia para ejecutar el juego

echo "🟦 Brick Tetris - Graphics Engine"
echo "===================================="
echo ""

# Check if the executable exists
if [ ! -f "runtime/build/bin/brick_tetris" ]; then
    echo "Error: Game executable not found!"
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

echo "Starting Tetris..."
echo "Controls: Arrow Keys/ASDF to move, W/UP to rotate, SPACE for hard drop, ESC to exit"
echo ""

cd "$(dirname "$0")"
./runtime/build/bin/brick_tetris

echo ""
echo "Thanks for playing! 🎮"

