# INSTRUCCIONES DE COMPILACIÓN - BRICKC

**Motor de Juegos de Ladrillos - Compilación con CMake**

---

## REQUISITOS PREVIOS

### macOS/Linux (Desarrollo)
- **CMake** versión 2.8 o superior
- **GCC** o **Clang** con soporte C++98
- **Make** (usualmente incluido)

### Windows XP (Target)
- **MinGW-w64** 4.x o superior
- **CMake** 2.8+
- **MSYS2** (recomendado) o **MinGW básico**

### Windows Moderno (Cross-compile para XP)
- **Visual Studio** 2003/2005 o **MinGW-w64**
- **CMake** 3.0+

---

## COMPILACIÓN PASO A PASO

### Opción 1: Compilación Básica (macOS/Linux)

```bash
# 1. Navegar al directorio del proyecto
cd brickc/

# 2. Crear directorio de build
mkdir build
cd build

# 3. Generar archivos de build
cmake ..

# 4. Compilar
make

# 5. El ejecutable estará en build/bin/brickc
```

### Opción 2: Compilación con Ninja (Más rápido)

```bash
cd brickc/
mkdir build
cd build

# Usar Ninja como generador
cmake .. -G Ninja
ninja

# Ejecutar
./bin/brickc
```

### Opción 3: Compilación para Windows XP (Cross-compile)

```bash
cd brickc/
mkdir build-xp
cd build-xp

# Configurar para Windows XP
cmake .. \
  -DCMAKE_SYSTEM_NAME=Windows \
  -DCMAKE_C_COMPILER=i686-w64-mingw32-gcc \
  -DCMAKE_CXX_COMPILER=i686-w64-mingw32-g++ \
  -DCMAKE_RC_COMPILER=i686-w64-mingw32-windres

make

# Resultado: bin/brickc.exe compatible con Windows XP
```

### Opción 4: Compilación Nativa en Windows XP

```cmd
REM Usar símbolo del sistema de Windows XP
cd brickc\
mkdir build
cd build

REM Con MinGW
cmake .. -G "MinGW Makefiles"
mingw32-make

REM Con Visual C++
cmake .. -G "NMake Makefiles"
nmake
```

---

## CONFIGURACIONES ESPECIALES

### Build de Release (Binario Pequeño)

```bash
cd brickc/build
cmake .. -DCMAKE_BUILD_TYPE=Release
make

# Verificar tamaño
ls -lh bin/brickc
```

### Build de Debug (Para Desarrollo)

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
make

# Ejecutar con debug
./bin/brickc --debug examples/snake.brick
```

### Build Estático (Sin dependencias externas)

```bash
# Para distribución independiente
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_STATIC=ON

make
```

---

## TARGETS DISPONIBLES

```bash
# Compilar solo el ejecutable principal
make brickc

# Compilar y copiar ejemplos
make copy_examples

# Crear paquete final para distribución
make package

# Limpiar archivos de build
make clean
```

---

## VERIFICACIÓN DE LA COMPILACIÓN

### 1. Probar Compilación Básica

```bash
# Verificar que el ejecutable existe
ls -la bin/brickc

# Probar versión
./bin/brickc --version

# Probar ayuda
./bin/brickc --help
```

### 2. Probar con Ejemplos

```bash
# Analizar ejemplo Snake
./bin/brickc examples/snake.brick

# Analizar ejemplo Tetris
./bin/brickc examples/tetris.brick

# Modo verboso
./bin/brickc --verbose examples/snake.brick
```

### 3. Probar Detección de Errores

```bash
# Debe mostrar errores claros
./bin/brickc tests/invalid_syntax.brick
./bin/brickc tests/invalid_lexical.brick
```

---

## SOLUCIÓN DE PROBLEMAS

### Error: "cmake: command not found"

**macOS:**
```bash
# Instalar con Homebrew
brew install cmake

# O con MacPorts
sudo port install cmake
```

**Ubuntu/Debian:**
```bash
sudo apt-get install cmake build-essential
```

**CentOS/RHEL:**
```bash
sudo yum install cmake gcc-c++ make
```

### Error: "No such file or directory: g++"

```bash
# macOS: Instalar Xcode Command Line Tools
xcode-select --install

# Linux: Instalar compilador
sudo apt-get install build-essential  # Ubuntu
sudo yum install gcc-c++              # CentOS
```

### Error de Compilación C++98

Si hay errores de compatibilidad C++98:
```bash
# Forzar estándar C++98
cmake .. -DCMAKE_CXX_FLAGS="-std=c++98 -pedantic"
make
```

### Warning: "ISO C++ forbids"

Es normal con C++98, pero se puede suprimir:
```bash
cmake .. -DCMAKE_CXX_FLAGS="-std=c++98 -Wno-long-long"
make
```

### Error de Linkeo en Windows XP

```cmd
REM Asegurar linkeo estático
cmake .. -DCMAKE_EXE_LINKER_FLAGS="-static-libgcc -static-libstdc++"
mingw32-make
```

---

## CONFIGURACIONES AVANZADAS

### Build con Información de Debug

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DCMAKE_CXX_FLAGS_RELWITHDEBINFO="-O2 -g"
make
```

### Build con Sanitizers (Debug)

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS="-fsanitize=address -g"
make
```

### Compilación Paralela (Más rápida)

```bash
# Usar todos los núcleos disponibles
make -j$(nproc)

# O especificar número de jobs
make -j4
```

---

## EMPAQUETADO PARA DISTRIBUCIÓN

### Crear Paquete Completo

```bash
cd brickc/build

# Generar paquete final
make package

# Verificar contenido
ls -la BRICK/
du -sh BRICK/  # Debe ser ≤ 1.44MB
```

### Contenido del Paquete Final

```
BRICK/
├── brickc.exe          # Ejecutable principal
├── examples/
│   ├── snake.brick     # Ejemplo Snake
│   └── tetris.brick    # Ejemplo Tetris
└── README.txt          # Manual de usuario
```

### Verificar Tamaño para Disquete

```bash
# Debe ser ≤ 1,474,560 bytes (1.44 MB)
du -b BRICK/ | tail -1

# Crear imagen de disquete (opcional)
dd if=/dev/zero of=brickc.img bs=1024 count=1440
mkfs.msdos brickc.img
mount brickc.img /mnt/floppy
cp -r BRICK/* /mnt/floppy/
umount /mnt/floppy
```

---

## TESTING AUTOMATIZADO

### Ejecutar Todos los Tests

```bash
# Desde el directorio build
../scripts/run_tests.sh

# O manualmente
for file in ../tests/*.brick; do
    echo "Testing $file..."
    ./bin/brickc "$file"
done
```

### Benchmark de Performance

```bash
# Medir tiempo de análisis
time ./bin/brickc examples/snake.brick
time ./bin/brickc examples/tetris.brick
```

---

## DESARROLLO INCREMENTAL

### Rebuild Rápido Durante Desarrollo

```bash
# Solo recompilar archivos modificados
make

# Rebuild completo si es necesario
make clean
make
```

### Debug de Problemas de Compilación

```bash
# Ver comandos de compilación detallados
make VERBOSE=1

# O con más información
cmake .. -DCMAKE_VERBOSE_MAKEFILE=ON
make
```

---

## INTEGRACIÓN CONTINUA

### Script de Build Automático

```bash
#!/bin/bash
# build.sh
set -e

echo "=== Compilando BrickC ==="
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
make package

echo "=== Testing básico ==="
./bin/brickc --version
./bin/brickc examples/snake.brick
./bin/brickc examples/tetris.brick

echo "=== Build exitoso ==="
echo "Ejecutable: $(ls -lh bin/brickc)"
echo "Paquete: $(du -sh BRICK/)"
```

---

**NOTAS IMPORTANTES:**

1. **Siempre probar** en el ambiente target (Windows XP) antes de la entrega
2. **Verificar tamaño** del paquete final ≤ 1.44MB  
3. **Probar casos de error** para asegurar mensajes claros
4. **Documentar** cualquier dependencia adicional requerida

---

**¡Listo para compilar!** 🚀

El proyecto está configurado para compilar desde el primer intento. Si encuentras algún problema, revisa la sección de solución de problemas o consulta los logs detallados con `make VERBOSE=1`.
