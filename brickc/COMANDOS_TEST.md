# COMANDOS DE TEST EJECUTADOS - BRICKC

## COMPILACIÓN
```bash
cd brickc/build
cmake ..
make
```

## TESTS BÁSICOS
```bash
./bin/brickc --version
./bin/brickc --help
./bin/brickc
```

## TESTS CON EJEMPLOS VÁLIDOS
```bash
./bin/brickc bin/examples/snake.brick
./bin/brickc --verbose bin/examples/snake.brick
./bin/brickc bin/examples/tetris.brick
./bin/brickc --json bin/examples/snake.brick
```

## TESTS DE ERRORES
```bash
./bin/brickc archivo_que_no_existe.brick
./bin/brickc bin/tests/invalid_syntax.brick
./bin/brickc bin/tests/invalid_lexical.brick
```

## VERIFICACIÓN DE ARCHIVOS
```bash
head -5 bin/examples/snake.brick
find . -name "*.brick"
ls -la bin/
```

## EMPAQUETADO Y VERIFICACIÓN DE TAMAÑO
```bash
make package
mkdir -p BRICK
cp bin/brickc BRICK/
cp -r bin/examples BRICK/
cp ../README.txt BRICK/
du -sh BRICK/
find BRICK/ -type f -exec ls -l {} \; | awk '{sum += $5} END {print "Tamaño total:", sum, "bytes"}'
```

## TEST DEL PAQUETE FINAL
```bash
cd BRICK/
./brickc --version
./brickc examples/snake.brick
./brickc examples/tetris.brick
```

## COMANDOS DE COMPILACIÓN ADICIONALES
```bash
which g++
g++ --version
cd /Users/dacastaneda/dgc7/game-bricks-armi/brickc && mkdir -p build
cp -r tests build/bin/
```
