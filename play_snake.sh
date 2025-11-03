#!/bin/bash

# Play Brick Snake Game
# Script de conveniencia para ejecutar el juego

echo "🐍 Brick Snake - Graphics Engine"
echo "=================================="
echo ""

# Check if the executable exists
if [ ! -f "runtime/build/bin/brick_snake" ]; then
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

echo "Starting game..."
echo "Controls: Arrow Keys/WASD to move, SPACE to pause, ESC to exit"
echo ""

cd "$(dirname "$0")"
./runtime/build/bin/brick_snake

echo ""
echo "Thanks for playing! 🎮"

