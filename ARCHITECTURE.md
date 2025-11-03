# Brick Game Engine - Arquitectura del Sistema

## Visión General

El proyecto **Brick Game Engine** implementa una cadena completa de compilación e interpretación de lenguaje DSL (Domain-Specific Language) a juegos gráficos:

```
┌─────────────────────────────────────────────────────────────┐
│                  BRICK GAME ENGINE                          │
└─────────────────────────────────────────────────────────────┘

        ENTRADA                 COMPILADOR              RUNTIME
        
  .brick files  ──►   Lexer  ──►  Parser  ──►  AST  ──►  Game Engine (SDL2)
                        │           │          │
                        └───────────┼──────────┘
                                    │
                            Symbol Table & Validation
                                    │
                            Ejecutable / Juego Gráfico
```

## Componentes del Sistema

### 1. FRONT-END: Compilador BrickC (Entrega 1 )

**Ubicación:** `su-ruta/game-bricks-armi/brickc/src/`

**Componentes:**
- **`lexer.cpp/hpp`** - Análisis léxico
  - Tokenización del código fuente
  - Manejo de colores hexadecimales, números, identificadores
  - Generación de errores léxicos

- **`parser.cpp/hpp`** - Análisis sintáctico
  - Construcción del AST (Abstract Syntax Tree)
  - Validación de gramática
  - Recuperación de errores

- **`ast.cpp/hpp`** - Árbol de Sintaxis Abstracta
  - Representación estructurada del programa
  - Tipos de nodos (game, entity, rule, control, etc.)

- **`symbols.cpp/hpp`** - Tabla de Símbolos
  - Almacenamiento de configuración del juego
  - Información de entidades
  - Mapeo de controles
  - Definiciones de reglas

- **`error.cpp/hpp`** - Sistema de errores
  - Mensajes de error formateados
  - Información de línea y columna
  - Clasificación (léxico, sintáctico, semántico)

- **`util.cpp/hpp`** - Utilidades
  - Funciones auxiliares
  - Conversión de datos

**Salida:** Información compilada en la tabla de símbolos que puede:
- Mostrarse en consola (formato texto)
- Exportarse a JSON (para depuración)
- Usarse por el runtime

### 2. BACK-END: Motor Gráfico SDL2 (Entrega 2 - NUEVA ✨)

**Ubicación:** `su-ruta/game-bricks-armi/runtime/`

#### 2.1 Motor Base (`GameEngine`)

```cpp
class GameEngine {
    // Gestión de ventana SDL2
    SDL_Window* window;
    SDL_Renderer* renderer;
    
    // Grilla y propiedades
    int gridWidth, gridHeight, cellSize;
    
    // Gestión de entidades
    std::map<string, Entity> entities;
    
    // Renderizado
    void renderGrid();
    void renderEntities();
};
```

**Responsabilidades:**
- Inicializar SDL2
- Crear ventana y renderer
- Mantener mapa de entidades
- Renderizar grilla y objetos del juego

#### 2.2 Sistema de Entrada (`InputManager`)

```cpp
class InputManager {
    // Estado de teclado
    const Uint8* currentKeyState;
    Uint8* previousKeyState;
    
    // Mapeo de acciones
    std::map<SDL_Keycode, string> keyToActionMap;
    
    // Queries
    bool isKeyDown(SDL_Keycode key);
    bool isActionActive(string action);
};
```

**Responsabilidades:**
- Procesar eventos SDL
- Detectar pulsaciones/liberaciones de teclas
- Mapear teclas a acciones del juego
- Proporcionar API de alto nivel

#### 2.3 Utilidades de Renderizado (`Renderer`)

```cpp
class Renderer {
    // Primitivas gráficas
    static void drawRect(SDL_Renderer*, int x, int y, int w, int h, ...);
    static void fillRect(...);
    static void drawCircle(...);
    static void fillCircle(...);
    static void drawLine(...);
};
```

**Responsabilidades:**
- Proporcionar funciones de bajo nivel para dibujar
- Abstracción de SDL_RenderDrawRect, SDL_RenderDrawPoint, etc.
- Manejo de colores y renderizado

#### 2.4 Implementación del Juego (`SnakeGame`)

```cpp
class SnakeGame {
    GameEngine engine;
    InputManager input;
    
    // Estado del juego
    std::deque<SnakeSegment> snake;
    Point fruitPosition, superfruitPosition;
    int score, lives, gameSpeed;
    Direction currentDirection, nextDirection;
    
    // Lógica
    void initializeGame();
    void updateSnake(float deltaTime);
    void checkCollisions();
    void spawnFruit();
    void handleGameOver();
};
```

**Responsabilidades:**
- Mantener estado del juego
- Procesar entrada del usuario
- Actualizar física/lógica de juego
- Detectar colisiones
- Renderizar el estado actual

### 3. Estructuras de Datos Comunes

#### Point
```cpp
struct Point {
    int x, y;
    bool operator==(const Point& other);
    bool operator!=(const Point& other);
};
```

#### Color
```cpp
struct Color {
    Uint8 r, g, b, a;
    static Color fromHex(unsigned int hex);  // #RRGGBB
};
```

#### Entity
```cpp
struct Entity {
    string name;
    Point position;
    Color color;
    int width, height;
    bool solid, visible;
};
```

## Flujo de Ejecución

### Compilación (BrickC)

```
snake.brick
    ↓
[Lexer] → tokens
    ↓
[Parser] → AST
    ↓
[Symbol Table] → validación y almacenamiento
    ↓
Salida (consola/JSON)
```

### Ejecución (Runtime)

```
Main.cpp
    ↓
SnakeGame::initialize()
    ├─ GameEngine::initialize() → SDL2, ventana, renderer
    ├─ InputManager → mapeo de teclas
    └─ initializeGame() → serpiente, frutas, etc.
    ↓
Main Loop (60 FPS)
    ├─ handleInput() → procesar SDL_Event
    ├─ updateSnake(deltaTime) → lógica de movimiento
    ├─ checkCollisions() → detección
    ├─ drawGame() → renderizar
    └─ [volver al inicio]
    ↓
GameOver
```

## Loop Principal del Juego

```cpp
void SnakeGame::run() {
    const float FPS = 60.0f;
    const float FRAME_TIME = 1.0f / FPS;
    
    while (engine.isRunning() && !input.shouldQuit()) {
        // 1. ENTRADA
        handleInput();  // Procesa eventos SDL
        
        // 2. LÓGICA
        updateSnake(deltaTime);   // Mueve serpiente
        checkCollisions();         // Detecta colisiones
        
        // 3. RENDERIZADO
        engine.render();  // Dibuja en pantalla
    }
}
```

## Sincronización con el Compilador

### Correspondencia entre BrickLang y Runtime

| Concepto BrickLang | Mapeo al Runtime |
|------------------|-----------------|
| `game NAME { }` | `SnakeGame` class |
| `speed: N` | `moveDelay = 1.0f / N` |
| `grid: (W, H)` | `GameEngine(W, H)` |
| `entity NAME` | `Entity` en `engine.entities` |
| `spawn: (X, Y)` | `entity.position` |
| `color: #RGB` | `Color::fromHex()` |
| `controls` | `InputManager::mapKeyToAction()` |
| `rule tick` | `updateSnake()` |
| `collide` | `checkCollisions()` |

## Optimizaciones Implementadas

### Rendimiento
-  **V-Sync** - Sincronización con pantalla (60 FPS)
-  **Hardware Acceleration** - Renderer acelerado
-  **Delta Time Capping** - Evita jumps grandes
-  **Efficient Collision Detection** - O(n) para self-collision

### Memoria
-  **Entities Map** - Acceso rápido a entidades
-  **Deque para Snake** - Inserción/eliminación eficiente
-  **Stack Allocation** - Minimización de dynamic alloc

### Usabilidad
- **Action Mapping** - Fácil configuración de controles
- **Flexible Grid System** - Tamaños configurables
- **Color System** - Soporte hexadecimal compatible con BrickLang

## Estructura de Directorios

```
/su-ruta/game-bricks-armi/
├── brickc/                          # Compilador
│   ├── src/                         # Fuente del compilador
│   ├── examples/                    # Archivos .brick de ejemplo
│   ├── tests/                       # Casos de prueba
│   └── CMakeLists.txt              # Build config
│
├── runtime/                         # Motor gráfico (NUEVO)
│   ├── include/                     # Headers
│   │   ├── game_engine.hpp
│   │   ├── renderer.hpp
│   │   ├── input_manager.hpp
│   │   └── snake_game.hpp
│   ├── src/                         # Implementación
│   │   ├── game_engine.cpp
│   │   ├── renderer.cpp
│   │   ├── input_manager.cpp
│   │   ├── snake_game.cpp
│   │   └── main.cpp
│   └── CMakeLists.txt              # Build config
│
├── GAME_ENGINE_README.md            # Documentación del motor
├── ARCHITECTURE.md                  # Este archivo
└── setup.sh                         # Script de setup
```

## Próximas Fases

### Entrega 3: Integración Completa
- [ ] Parser genera bytecode/configuración para runtime
- [ ] Runtime interpreta configuración de archivos .brick compilados
- [ ] Sistema plugin para custom game logic
- [ ] Modo editor visual

### Entrega 4: Características Avanzadas
- [ ] Sonido y música (SDL_mixer)
- [ ] Sprites y animaciones
- [ ] Múltiples niveles
- [ ] Menú principal
- [ ] Guardado de scores

### Entrega 5: Herramientas
- [ ] IDE web para BrickLang
- [ ] Debugger visual
- [ ] Asset manager
- [ ] Publicación de juegos

## Notas Técnicas

### Compatibilidad
- **C++11** - Compatible con compilers modernos
- **SDL2** - Cross-platform (Windows, macOS, Linux)
- **CMake** - Build system estándar

### Rendimiento Target
- **60 FPS** - Frame rate consistente
- **30x30 grid** - Resolución del juego
- **20px cells** - Resolución renderizada (600x600)

### Decisiones de Diseño

1. **GameEngine separado de SnakeGame**
   - Permite reutilización para otros juegos
   - Separación clara de responsabilidades

2. **InputManager como clase separada**
   - Abstracción de SDL events
   - Fácil de testear y reemplazar

3. **Renderer con funciones estáticas**
   - No requiere instancia
   - API simple y eficiente

4. **SnakeGame como orchestrator principal**
   - Coordina engine e input
   - Mantiene lógica del juego

---

**Documentación de Arquitectura**  
*Motor de Juegos de Ladrillos - Brick Game Engine*  
*Noviembre 2024*

