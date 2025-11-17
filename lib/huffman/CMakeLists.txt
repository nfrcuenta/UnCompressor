# UnCompressor/lib/huffman/CMakeLists.txt

cmake_minimum_required(VERSION 3.10)

# Nombre del sub-proyecto (útil para IDEs)
project(huffman_module LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 1. Creación de la biblioteca ---
# NOTA: Cambié HuffmanEncoder.cpp por MatrixHuffman.cpp según nuestro código anterior
add_library(huffman_lib
    src/HuffmanNode.cpp
    src/HuffmanTree.cpp
    src/MatrixHuffman.cpp
)

# Definir dónde están los headers para quien use esta librería
target_include_directories(huffman_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# --- 2. Ejecutable de Prueba (Solo para desarrollo) ---
# Creamos un ejecutable que usa tu librería
add_executable(huffman_test tests/test_main.cpp)

# Vinculamos el ejecutable con tu librería
target_link_libraries(huffman_test PRIVATE huffman_lib)
