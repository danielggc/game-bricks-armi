# BrickC - Compilador de BrickLang

Compilador para el lenguaje de programación de juegos BrickLang.

## Flujo de Compilación

1. **Lexer** → Convierte código fuente en tokens
2. **Parser** → Construye árbol sintáctico (AST)  
3. **Symbols** → Crea tabla de símbolos
4. **Validator** → Valida reglas semánticas

## Uso

```bash
# Compilar el proyecto
cd brickc/build
cmake ..
make

# Ejecutar compilador
./bin/brickc examples/snake.brick
./bin/brickc --verbose examples/snake.brick
./bin/brickc --json examples/snake.brick
```

## Estructura del Proyecto

```
brickc/
├── src/           # Código fuente del compilador
├── examples/      # Archivos .brick de ejemplo
├── tests/         # Casos de prueba
└── build/         # Archivos compilados
```

## Ejemplo de Código BrickLang

```brick
game snake {
    speed: 8;
    lives: 3;
    score: 0;
    
    entity head {
        color: #00FF00;
        solid: true;
    }
    
    rule movement {
        collide: head, wall: lose_life();
    }
}
```

## Capturas de Pantalla

### Compilación Exitosa
![Compilación del proyecto](Captura%20de%20pantalla%202025-10-22%20a%20la(s)%206.13.56%20p.m..png)

### Ejecución del Compilador
![Ejecución con verbose](Captura%20de%20pantalla%202025-10-22%20a%20la(s)%206.14.40%20p.m..png)  
