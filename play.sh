#!/bin/bash

# Script principal para ejecutar Brick Games Engine
# Sistema unificado con menú gráfico SDL

echo "╔════════════════════════════════════════════════════════════╗"
echo "║                                                            ║"
echo "║             BRICK GAMES ENGINE - SISTEMA LIMPIO            ║"
echo "║                                                            ║"
echo "║              Menú Gráfico SDL Unificado                   ║"
echo "║                                                            ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""

# Verificar que el ejecutable existe
if [ ! -f "runtime/build/bin/brick_menu" ]; then
    echo "Error: brick_menu no encontrado"
    echo "   Compilando el sistema..."
    cd runtime/build
    cmake .. && make
    cd ../..
fi

echo "Iniciando Brick Games Engine..."
echo ""
echo "Características del menú:"
echo "  - Interfaz gráfica espacial animada"
echo "  - Campo de estrellas en movimiento"
echo "  - Texto pixel art sin imágenes externas"
echo "  - Escaneo automático de archivos .brick"
echo "  - Carga dinámica de configuraciones"
echo ""
echo "Controles:"
echo "  - Flechas arriba/abajo: Navegar entre juegos"
echo "  - ENTER: Seleccionar juego"
echo "  - ESC: Salir"
echo ""

# Ejecutar el menú
cd runtime/build
./bin/brick_menu

echo ""
echo "Gracias por jugar Brick Games!"