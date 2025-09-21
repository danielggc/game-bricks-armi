# BrickLang - Especificación del Lenguaje

**Motor de Juegos de Ladrillos - Entrega 1**  
*Especificación Técnica del Lenguaje de Dominio Específico*

---

## Índice

1. [Introducción](#introducción)
2. [Análisis Léxico](#análisis-léxico)
3. [Análisis Sintáctico](#análisis-sintáctico)
4. [Tabla de Símbolos](#tabla-de-símbolos)
5. [Ejemplos](#ejemplos)
6. [Manejo de Errores](#manejo-de-errores)

---

## Introducción

BrickLang es un lenguaje específico de dominio (DSL) diseñado para definir juegos tipo arcade de manera declarativa. El lenguaje permite especificar:

- **Configuración del juego**: velocidad, tamaño de grilla, colores
- **Entidades**: elementos del juego con propiedades específicas
- **Controles**: mapeo de teclas a acciones
- **Reglas**: lógica del juego mediante eventos y acciones

### Filosofía del Diseño

- **Declarativo**: Se describe QUÉ debe pasar, no CÓMO
- **Minimalista**: Sintaxis simple y expresiva
- **Específico**: Optimizado para juegos de grilla/ladrillos
- **Extensible**: Preparado para futuras expansiones

---

## Análisis Léxico

### Tokens del Lenguaje

#### Palabras Clave (Keywords)
```
game      speed     grid      color     entity
spawn     solid     controls  rule      tick
collide   score     lose      true      false
```

#### Identificadores
- Empiezan con letra o underscore
- Pueden contener letras, números y underscores
- Ejemplos: `snake_head`, `player1`, `moveUp`

#### Literales

**Números enteros:**
```
0  1  42  1000
```

**Colores hexadecimales:**
```
#FF0000   #00FF00   #0000FF   #FFFFFF
```

**Booleanos:**
```
true  false
```

#### Operadores y Símbolos
```
{  }    llaves
(  )    paréntesis
:       dos puntos
;       punto y coma
,       coma
=       asignación
+  -    suma y resta
*  /    multiplicación y división
<  >    menor y mayor que
<= >=   menor/mayor o igual
== !=   igual y diferente
```

#### Comentarios
```cpp
// Comentario de línea hasta el final
```

### Reglas Lexicográficas

1. **Case-sensitive**: `Game` ≠ `game`
2. **Espacios en blanco**: ignorados (espacios, tabs, saltos de línea)
3. **Comentarios**: desde `//` hasta fin de línea
4. **Colores**: formato `#RRGGBB` con dígitos hexadecimales válidos

---

## Análisis Sintáctico

### Gramática (EBNF)

```ebnf
Program     := "game" IDENT "{" GameBody "}"

GameBody    := Settings Entities Controls Rules

Settings    := { SettingStmt }
SettingStmt := ("speed" | "grid" | "color") ":" Value ";"

Entities    := { EntityDecl }
EntityDecl  := "entity" IDENT "{" { EntityProp } "}"
EntityProp  := ("spawn" | "color" | "solid") ":" Value ";"

Controls    := "controls" "{" { ControlStmt } "}"
ControlStmt := IDENT ":" IDENT ";"

Rules       := { RuleDecl }
RuleDecl    := "rule" IDENT "{" { RuleStmt } "}"
RuleStmt    := TickStmt | CollideStmt | ScoreStmt | LoseStmt | RuleDecl

TickStmt    := "tick" ":" IDENT "(" [ ArgList ] ")" ";"
CollideStmt := "collide" ":" IDENT "," IDENT ":" IDENT "(" [ ArgList ] ")" ";"
ScoreStmt   := "score" ":" ("+" | "-") INT ";"
LoseStmt    := "lose" ";"

Value       := INT | HEXCOLOR | BOOL | Point | Expression
Point       := "(" INT "," INT ")"
Expression  := Term { ("+" | "-") Term }
Term        := Factor { ("*" | "/") Factor }
Factor      := INT | IDENT | "(" Expression ")"

ArgList     := Expression { "," Expression }
```

### Precedencia de Operadores

```
1. ( )          paréntesis
2. * /          multiplicación, división
3. + -          suma, resta
4. < > <= >=    comparación
5. == !=        igualdad
```

---

## Tabla de Símbolos

La tabla de símbolos almacena toda la información semántica del juego:

### Estructura de la Tabla

```cpp
class SymbolTable {
    // Configuración del juego
    string gameName;
    int speed;
    int gridWidth, gridHeight;
    unsigned int gameColor;
    
    // Entidades
    map<string, Entity> entities;
    
    // Controles
    map<string, Control> controls;
    
    // Reglas
    map<string, Rule> rules;
};
```

### Información de Entidades

```cpp
class Entity {
    string name;
    Point spawn;        // Posición inicial
    unsigned int color; // Color en formato RGB
    bool solid;         // Es sólido?
    bool isDefined;     // Está definido?
};
```

### Información de Controles

```cpp
class Control {
    string input;   // Tecla o entrada
    string action;  // Acción a ejecutar
};
```

### Información de Reglas

```cpp
class Rule {
    string name;
    vector<ASTNode*> statements; // Lista de declaraciones
};
```

### Validaciones Semánticas

1. **Entidades duplicadas**: No se pueden definir dos entidades con el mismo nombre
2. **Controles duplicados**: No se pueden mapear dos acciones a la misma tecla
3. **Referencias válidas**: Las entidades en `collide` deben existir
4. **Valores en rango**: Coordenadas y colores dentro de límites válidos
5. **Configuración obligatoria**: Juego debe tener nombre

---

## Ejemplos

### Ejemplo Básico: Snake

```brick
game snake {
    speed: 8;
    grid: (20, 20);
    color: #001122;

    entity snake_head {
        spawn: (10, 10);
        color: #00FF00;
        solid: true;
    }

    entity fruit {
        spawn: (15, 15);
        color: #FF0000;
        solid: false;
    }

    controls {
        up: turn_up;
        down: turn_down;
        left: turn_left;
        right: turn_right;
    }

    rule movement {
        tick: move_forward();
        collide: snake_head, fruit: grow();
        collide: snake_head, fruit: score: +10;
    }
}
```

### Ejemplo Avanzado: Tetris

```brick
game tetris {
    speed: 5;
    grid: (10, 20);
    color: #112244;

    entity board {
        color: #333333;
        solid: true;
    }

    entity tetromino {
        spawn: (5, 0);
        color: #00FFFF;
        solid: true;
    }

    controls {
        left: move_left;
        right: move_right;
        down: soft_drop;
        up: rotate_cw;
        space: hard_drop;
    }

    rule gravity {
        tick: apply_gravity();
        collide: tetromino, board: lock_piece();
    }

    rule line_clearing {
        tick: check_complete_lines();
        score: +100;
    }
}
```

### Ejemplo con Expresiones

```brick
game math_game {
    speed: 5 + 3;
    grid: (10 * 2, 20 / 2);
    
    entity player {
        spawn: (5 + 5, 10 - 2);
    }
    
    rule scoring {
        score: +(10 * level);
        tick: update_time(current_time + 1);
    }
}
```

---

## Manejo de Errores

### Tipos de Errores

#### Errores Léxicos
```
L3:C15: carácter inválido '@' en identificador
L5:C8: color hexadecimal inválido '#GGGGGG'
L7:C20: número mal formado '123abc'
```

#### Errores Sintácticos
```
L4:C12: se esperaba ';' después del valor
L8:C5: se esperaba '}' para cerrar entidad
L12:C15: se esperaba ')' en expresión
```

#### Errores Semánticos
```
L10:C8: entidad 'player' ya fue definida en línea 5
L15:C20: entidad 'enemy' no está definida
L18:C12: control 'up' ya fue mapeado en línea 14
```

### Format de Mensajes de Error

```
L[línea]:C[columna]: [mensaje descriptivo]
```

### Recuperación de Errores

El parser implementa recuperación de errores para continuar el análisis después de encontrar un error, permitiendo detectar múltiples errores en una sola pasada.

### Estrategias de Sincronización

1. **Punto y coma**: Sincronizar en `;` para declaraciones
2. **Llaves**: Sincronizar en `}` para bloques
3. **Palabras clave**: Sincronizar en `entity`, `rule`, etc.

---

## Extensiones Futuras

### Entrega 2: Intérprete
- Ejecución de acciones `tick` y `collide`
- Simulación paso a paso en modo texto
- Sistema de eventos y callbacks

### Entrega 3: Motor Gráfico
- Renderizado en modo gráfico
- Sprites y animaciones básicas
- Input handling en tiempo real

---

## Implementación Técnica

### Arquitectura del Compilador

```
Archivo .brick
      ↓
   Lexer (lexer.cpp)
      ↓
   Tokens
      ↓  
   Parser (parser.cpp)
      ↓
   AST (ast.cpp)
      ↓
   SymbolTable (symbols.cpp)
      ↓
   Validación y Output
```

### Consideraciones de Rendimiento

- **Memoria**: Estructuras optimizadas para ≤1.44 MB total
- **Velocidad**: Análisis lineal O(n) donde n = tamaño del archivo
- **Compatibilidad**: C++98 para soporte en Windows XP

### Limitaciones Actuales

- Sin funciones definidas por usuario
- Sin variables globales o locales
- Sin estructuras de control (if, while, for)
- Sin importación de módulos o archivos externos

---

**Documento técnico - Motor de Juegos de Ladrillos**  
*Entrega 1: Analizador de Lenguaje y Tabla de Símbolos*  
*Septiembre 2024*
