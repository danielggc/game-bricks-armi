# Brick Game Engine - Motor Gráfico SDL2

## Descripción General

El Brick Game Engine es un motor gráfico moderno construido con **SDL2** que proporciona:

-  **Renderizado en tiempo real** con cuadrícula personalizable
-  **Sistema de entrada** flexible con mapeo de acciones
-  **Juego Snake completo** como ejemplo de implementación
-  **Sistema de colores** compatible con valores hexadecimales
-  **Gestión de entidades** y colisiones

## Arquitectura

```
runtime/
├── include/
│   ├── game_engine.hpp      # Motor gráfico principal
│   ├── renderer.hpp         # Utilidades de renderizado
│   ├── input_manager.hpp    # Gestión de entrada
│   └── snake_game.hpp       # Lógica del juego Snake
├── src/
│   ├── game_engine.cpp
│   ├── renderer.cpp
│   ├── input_manager.cpp
│   ├── snake_game.cpp
│   └── main.cpp             # Punto de entrada
└── CMakeLists.txt           # Configuración de compilación
```

## Requisitos

### Dependencias del Sistema
- **CMake** 3.10 o superior
- **SDL2** (libsdl2-dev en Debian/Ubuntu, sdl2 en macOS con Homebrew)
- **Compilador C++11** (GCC, Clang, MSVC)

### Instalación de SDL2

**macOS (Homebrew):**
```bash
brew install sdl2
```

**Ubuntu/Debian:**
```bash
sudo apt-get install libsdl2-dev
```

**Windows (MSVC):**
Descargar desde https://www.libsdl.org/download-2.0.php

## Compilación

### Opción 1: Compilación Estándar

```bash
cd /Users/dacastaneda/dgc7/game-bricks-armi/brickc/build
cmake ..
make
```

### Opción 2: Compilación Específica del Runtime

```bash
cd /Users/dacastaneda/dgc7/game-bricks-armi/runtime
mkdir -p build
cd build
cmake ..
make
```

## Ejecución

### Jugar Snake

```bash
./bin/brick_snake
```

O después de compilar desde el directorio build:

```bash
cd /Users/dacastaneda/dgc7/game-bricks-armi/brickc/build
make
./bin/brick_snake
```

## Controles del Juego Snake

| Tecla | Acción |
|-------|--------|
| W | Mover arriba |
|  S | Mover abajo |
|  A | Mover izquierda |
|  D | Mover derecha |
| ESPACIO | Pausar/Reanudar |
| ESC | Salir del juego |

## Mecánicas del Juego

### Sistema de Puntuación
-  **Fruta Regular** (Roja): +10 puntos
-  **Super Fruta** (Amarilla): +50 puntos + 3 segmentos extras

### Sistema de Vidas
- Comienza con **3 vidas**
- Se pierde una vida por:
  - Colisión con pared
  - Colisión consigo mismo
- Game Over cuando vidas llegan a 0

### Dificultad Progresiva
- La velocidad aumenta cada vez que comes una fruta regular
- Velocidad máxima: 15 (desde 8)

## Componentes Principales

### GameEngine
Motor gráfico principal que maneja:
- Inicialización de SDL2
- Gestión de la ventana y renderer
- Renderizado de grilla y entidades
- Gestión de propiedades del juego

```cpp
GameEngine engine(gridWidth, gridHeight, cellSize);
engine.initialize();
engine.render();
```

### InputManager
Sistema de entrada que proporciona:
- Detección de pulsación/liberación de teclas
- Mapeo de acciones
- Manejo de eventos

```cpp
input.mapKeyToAction(SDLK_UP, "up");
if (input.isActionActive("up")) { /* ... */ }
```

### Renderer
Utilidades de renderizado de bajo nivel:
- Rectángulos (contorno y relleno)
- Líneas
- Círculos (contorno y relleno)
- Limpieza y presentación

```cpp
Renderer::fillRect(renderer, x, y, w, h, r, g, b);
Renderer::drawCircle(renderer, cx, cy, radius, r, g, b);
```

### SnakeGame
Implementación del juego completo:
- Lógica de movimiento de serpiente
- Detección de colisiones
- Generación de frutas
- Gestión del estado del juego

## Estructura de Datos

### Point
Representa una posición en la grilla:
```cpp
struct Point {
    int x, y;
};
```

### Color
Valores RGBA:
```cpp
struct Color {
    Uint8 r, g, b, a;
    
    // Crear desde color hexadecimal
    Color::fromHex(0xFF0000); // Rojo
};
```

### Entity
Objeto renderizable en el juego:
```cpp
struct Entity {
    std::string name;
    Point position;
    Color color;
    int width, height;
    bool solid;
    bool visible;
};
```

## Loop Principal del Juego

```cpp
while (engine.isRunning() && !gameOver) {
    // 1. Procesar entrada
    handleInput();
    
    // 2. Actualizar lógica
    updateSnake(deltaTime);
    checkCollisions();
    
    // 3. Renderizar
    engine.render();
}
```

## Extensibilidad

### Crear un Nuevo Juego

1. **Extender de GameEngine:**
```cpp
class MyGame {
    GameEngine engine;
    InputManager input;
    
    bool initialize() {
        if (!engine.initialize()) return false;
        input.mapKeyToAction(SDLK_SPACE, "jump");
        return true;
    }
};
```

2. **Implementar lógica de juego:**
```cpp
void update(float deltaTime) {
    handleInput();
    updateGameLogic(deltaTime);
}
```

3. **Renderizar:**
```cpp
void render() {
    engine.render();
}
```

## Optimizaciones Implementadas

-  **V-Sync habilitado** para fluidez consistente
-  **Renderer acelerado por hardware** cuando disponible
-  **Blend mode** para transparencia
-  **Detección de colisiones optimizada**
-  **Frame time capping** para deltaTime consistente

## Troubleshooting

### SDL2 no encontrado
```bash
# Instalar SDL2 development files
# macOS:
brew install sdl2

# Ubuntu/Debian:
sudo apt-get install libsdl2-dev

# Después, limpiar CMake cache:
cd build
rm -rf CMakeCache.txt CMakeFiles/
cmake ..
```

### Error de compilación C++11
```bash
# Asegurate que el compilador soporta C++11
gcc --version  # Debe ser 4.7+
clang --version  # Debe ser 3.3+
```

### Ventana no se abre
```bash
# Verificar SDL2:
sdl2-config --version

# Si no existe, reinstalar SDL2
brew reinstall sdl2
```

## Próximas Mejoras

- [ ] Soporte para múltiples juegos
- [ ] Sistema de sprites
- [ ] Animaciones
- [ ] Sonido y música (SDL_mixer)
- [ ] Guardado de puntuaciones
- [ ] Menú principal
- [ ] Niveles con obstáculos
- [ ] Multiplayer

## Licencia

Parte del proyecto Brick Games - Motor de Juegos de Ladrillos

## Autor

Equipo de desarrollo - BrickLang Game Engine

---

**¡Diviértete jugando Snake!** 🐍✨

