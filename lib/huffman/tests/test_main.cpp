//test_main.cpp
#include <iostream>
#include <vector>
#include <iomanip> // Para formatear la salida

// Incluimos TU librería
#include "huffman/MatrixHuffman.hpp"

int main() {
    std::cout << "=== TEST: Modulo de Huffman para Matriz Dispersa ===\n\n";

    // 1. SIMULACIÓN DE DATOS DE ENTRADA
    // Imaginemos una matriz original de 5x5 (25 celdas).
    // El fondo es "2a".
    int filas = 5;
    int cols = 5;
    std::string fondo = "2a";
    std::string archivoSalida = "test_output.txt";

    // Datos dispersos (Tripletas)
    std::vector<huffman::Triplete> entrada = {
        {0, 0, "14f"}, // (0,0) valor raro
        {1, 3, "11"},  // (1,3) valor raro
        {2, 2, "14f"}, // (2,2) repeticion de 14f
        {4, 4, "XYZ"}  // (4,4) valor muy raro
    };

    // 2. EJECUCIÓN DE LA LIBRERÍA
    std::cout << "[INFO] Procesando matriz de " << filas << "x" << cols << "...\n";
    
    auto diccionario = huffman::procesarMatrizYExportar(
        entrada, 
        fondo, 
        filas, 
        cols, 
        archivoSalida
    );

    // 3. VERIFICACIÓN DE RESULTADOS
    std::cout << "\n[RESULTADO 1] Diccionario Generado:\n";
    std::cout << "-----------------------------------\n";
    for (const auto& par : diccionario) {
        std::cout << " Simbolo: " << std::setw(5) << par.first 
                  << " | Codigo Huffman: " << par.second << "\n";
    }

    // Verificación rápida lógica:
    // '2a' debería tener el código más corto porque hay (25 - 4 = 21) '2a's.
    std::cout << "\n[CHECK] El codigo para '" << fondo << "' es: " << diccionario[fondo] << "\n";
    if (diccionario[fondo].length() <= 1) {
        std::cout << " -> EXITO: El fondo tiene un codigo corto.\n";
    } else {
        std::cout << " -> ALERTA: El codigo de fondo parece largo. Revisar logica de frecuencias.\n";
    }

    std::cout << "\n[RESULTADO 2] Archivo '" << archivoSalida << "' generado.\n";
    std::cout << "Revisa su contenido. Deberia ser una matriz de ceros y unos.\n";

    return 0;
}