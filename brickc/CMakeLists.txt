# CMakeLists.txt para BrickC - Compatible con Windows XP
cmake_minimum_required(VERSION 3.5)
project(BrickC)

# C++11 para compatibilidad moderna (funciona en XP con MinGW moderno)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Configuración para binarios pequeños
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os -fno-exceptions -fno-rtti")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s -DNDEBUG")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
elseif(MSVC)
    # Visual C++ 6.0 / Visual Studio 2003 compatible
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Os /GR- /EHs-c-")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# Directorio de código fuente
set(SRC_DIR src)

# Archivos fuente
set(SOURCES
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/lexer.cpp
    ${SRC_DIR}/parser.cpp
    ${SRC_DIR}/ast.cpp
    ${SRC_DIR}/symbols.cpp
    ${SRC_DIR}/error.cpp
    ${SRC_DIR}/util.cpp
)

# Archivos header
set(HEADERS
    ${SRC_DIR}/lexer.hpp
    ${SRC_DIR}/parser.hpp
    ${SRC_DIR}/ast.hpp
    ${SRC_DIR}/symbols.hpp
    ${SRC_DIR}/error.hpp
    ${SRC_DIR}/util.hpp
)

# Ejecutable principal
add_executable(brickc ${SOURCES} ${HEADERS})

# Configuraciones específicas para Windows XP
if(WIN32)
    # Asegurar compatibilidad con Windows XP
    target_compile_definitions(brickc PRIVATE 
        WINVER=0x0501 
        _WIN32_WINNT=0x0501
        _WIN32_WINDOWS=0x0501
        _WIN32_IE=0x0600
    )
    
    # Enlazado estático para evitar dependencias de runtime
    if(CMAKE_COMPILER_IS_GNUCXX)
        target_link_libraries(brickc -static-libgcc -static-libstdc++)
    endif()
endif()

# Directorio de salida
set_target_properties(brickc PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Target para copiar ejemplos al directorio de salida
add_custom_target(copy_examples ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/examples
        ${CMAKE_BINARY_DIR}/bin/examples
    DEPENDS brickc
    COMMENT "Copiando ejemplos al directorio de salida"
)

# Target para crear el paquete final (≤1.44MB)
add_custom_target(package
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/BRICK
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/bin/brickc ${CMAKE_BINARY_DIR}/BRICK/
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/bin/examples ${CMAKE_BINARY_DIR}/BRICK/examples
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/README.txt ${CMAKE_BINARY_DIR}/BRICK/
    DEPENDS brickc copy_examples
    COMMENT "Creando paquete final BRICK/"
)

# Información de tamaño del ejecutable
add_custom_command(TARGET brickc POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Tamaño del ejecutable:"
    COMMAND ${CMAKE_COMMAND} -E echo "======================================"
    COMMAND ls -lh $<TARGET_FILE:brickc> || dir /-c $<TARGET_FILE:brickc>
    COMMENT "Mostrando tamaño del ejecutable"
)
