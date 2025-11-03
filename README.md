# BrickC - Compilador de BrickLang y Motor Gráfico SDL2

## ¿Qué es Brick Games?

**Brick Games** es un **motor de juegos completo** que incluye:


## Características

### Compilador BrickLang
-  Análisis léxico, sintáctico y semántico
-  Generación de tabla de símbolos
-  Sistema robusto de manejo de errores
-  Soporte para colores hexadecimales
-  Expresiones matemáticas en configuración

### Motor Gráfico
-  Renderizado en tiempo real con SDL2
-  Sistema flexible de entrada (teclado)
-  Grilla personalizable
-  Sistema de colores RGBA
-  60 FPS con sincronización V-Sync

### Juego Snake
-  Serpiente progresiva
-  Frutas regulares (+10 pts)
-  Super frutas (+50 pts, +3 segmentos)
-  Sistema de 3 vidas
-  Dificultad progresiva

## Instalación Rápida

### Opción 1: Script Automático (Recomendado)

```bash
./setup.sh
```

Este script:
- Instala SDL2 automáticamente
- Instala dependencias de compilación
- Compila el proyecto completo

### Opción 2: Compilación Manual

```bash
# Instalar SDL2
brew install sdl2  # macOS
# O: sudo apt-get install libsdl2-dev  # Ubuntu/Debian

# Compilar
cd runtime
mkdir -p build
cd build
cmake ..
make
```

## Ejecutar el Juego

### Método 1: Script de Conveniencia
```bash
./play_snake.sh
```

### Método 2: Directamente
```bash
./runtime/build/bin/brick_snake
```

## Controles del Juego

| Tecla | Acción |
|-------|--------|
|  W | Mover arriba |
|  S | Mover abajo |
|  A | Mover izquierda |
|  D | Mover derecha |
| ESPACIO | Pausar/Reanudar |
| ESC | Salir |

## Estructura del Proyecto

```
/Users/dacastaneda/dgc7/game-bricks-armi/
│
├── brickc/                    # Compilador BrickLang (Entrega 1)
│   ├── src/                   # Código fuente
│   ├── examples/              # Ejemplos .brick
│   ├── tests/                 # Casos de prueba
│   └── build/                 # Compilados
│
├── runtime/                   # Motor Gráfico SDL2 (NUEVO)
│   ├── include/               # Headers
│   ├── src/                   # Implementación
│   ├── build/                 # Compilados
│   └── CMakeLists.txt
│
├── GAME_ENGINE_README.md      # Documentación completa del motor
├── ARCHITECTURE.md            # Diseño del sistema
├── setup.sh                   # Script de instalación
└── play_snake.sh              # Script para jugar
```

## Flujo Compilación → Ejecución

```
snake.brick (código BrickLang)
    ↓
[BrickC Compiler] → Tabla de Símbolos
    ↓
[Runtime Engine] → Renderizado SDL2
    ↓
Juego ejecutándose 
```

## Uso del Compilador

```bash
# Compilar un archivo .brick
./brickc/build/bin/brickc examples/snake.brick

# Con modo verbose
./brickc/build/bin/brickc --verbose examples/snake.brick

# Exportar a JSON
./brickc/build/bin/brickc --json examples/snake.brick
```

## Ejemplo BrickLang

```brick
game snake_plus {
    speed: 8;
    grid: (30, 30);
    color: #001122;

    entity head {
        spawn: (15, 15);
        color: #00FF00;
        solid: true;
    }

    entity fruit {
        spawn: (15, 15);
        color: #FF0000;
        solid: false;
    }

    controls {
        w: turn_up;
        a: turn_left;
        s: turn_down;
        d: turn_right;
    }

    rule movement {
        tick: move_forward();
        collide: head, wall: lose_life();
    }
}
```

## Componentes Principales

### GameEngine
Motor gráfico base que proporciona:
- Inicialización SDL2
- Gestión de ventana y renderer
- Renderizado de grilla
- Gestión de entidades

### InputManager
Sistema de entrada flexible:
- Detección de pulsaciones/liberaciones
- Mapeo de acciones
- Manejo de eventos SDL

### Renderer
Utilidades de renderizado de bajo nivel:
- Rectángulos y círculos
- Líneas y primitivas
- Manejo de colores

### SnakeGame
Implementación del juego completo:
- Lógica de movimiento
- Detección de colisiones
- Generación de frutas
- Gestión de puntuación y vidas

## Requisitos del Sistema

- **macOS** 10.12+ (Intel/Apple Silicon) o **Ubuntu 18.04+** o **Windows 10+**
- **CMake** 3.10+
- **Compilador C++11** (GCC 4.7+, Clang 3.3+, MSVC 2015+)
- **SDL2** 2.0+

## Documentación Completa

Para más detalles, consulta:
- 📖 [GAME_ENGINE_README.md](GAME_ENGINE_README.md) - Guía completa del motor gráfico
- 🏗️ [ARCHITECTURE.md](ARCHITECTURE.md) - Diseño técnico del sistema
- 📋 [produc_spec.md](produc_spec.md) - Especificación del lenguaje BrickLang


## Troubleshooting

### SDL2 no se encuentra
```bash
brew reinstall sdl2
# O en Linux:
sudo apt-get install libsdl2-dev
```

### Error de compilación
```bash
cd runtime/build
rm -rf CMakeCache.txt CMakeFiles/
cmake ..
make clean
make
```

## Autor

Equipo de desarrollo - BrickLang Game Engine

## Licencia

Parte del proyecto Brick Games - Motor de Juegos de Ladrillos

---

## Screenshots

###  Video Demostración
Mira el proyecto en acción:
- [Video 1 en Google Drive](https://drive.google.com/file/d/1wAgLsYLOasJPuwe_jIHP58uC-Yt025a_/view?usp=share_link)
- [Video 2 en Google Drive](https://drive.google.com/file/d/1wAgLsYLOasJPuwe_jIHP58uC-Yt025a_/view?usp=sharing)

###  Capturas del Proyecto

**Ejecución del Juego - Snake**
![Juego Snake Corriendo](Captura%20de%20pantalla%202025-10-22%20a%20la(s)%206.13.56%20p.m..png)

**Interfaz del Juego**
![Interfaz Juego](Captura%20de%20pantalla%202025-10-22%20a%20la(s)%206.14.40%20p.m..png)

**Compilador BrickC - Análisis Léxico**
![BrickC Lexer](Captura%20de%20pantalla%202025-11-03%20a%20la(s)%201.08.01%20p.m..png)

**Compilador BrickC - Análisis Sintáctico**
![BrickC Parser](Captura%20de%20pantalla%202025-11-03%20a%20la(s)%201.08.47%20p.m..png)

**Sistema Completo en Ejecución**
![Sistema Completo](Captura%20de%20pantalla%202025-11-03%20a%20la(s)%201.09.13%20p.m..png)

---

**¡Diviértete creando y jugando tus propios juegos con Brick Games!** 🎮✨   
