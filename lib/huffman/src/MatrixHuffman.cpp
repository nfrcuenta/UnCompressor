//MatrixHuffman.cpp
// 1. Incluimos la definición de este módulo
#include "huffman/MatrixHuffman.hpp"

// 2. Incluimos el árbol de Huffman para poder usarlo
#include "huffman/HuffmanTree.hpp"

// 3. Librerías estándar necesarias
#include <iostream>
#include <fstream>
#include <vector>

namespace huffman {

std::map<std::string, std::string> procesarMatrizYExportar(
    const std::vector<Triplete>& datosDispersos,
    const std::string& valorMasFrecuente,
    int totalFilas,
    int totalCols,
    const std::string& nombreArchivoSalida)
{
    // --- PASO 1: Calcular Frecuencias ---
    std::map<std::string, int> frecuencias;

    // A. Contar los elementos que SÍ están en la lista dispersa
    for (const auto& item : datosDispersos) {
        frecuencias[item.valor]++;
    }

    // B. Calcular cuántas veces aparece el valor de fondo ("2a")
    // Total celdas = filas * columnas
    long long totalCeldas = (long long)totalFilas * totalCols;
    long long celdasOcupadas = datosDispersos.size();
    long long celdasVacias = totalCeldas - celdasOcupadas;
    
    if (celdasVacias > 0) {
        frecuencias[valorMasFrecuente] += celdasVacias;
    }

    // --- PASO 2: Generar Huffman ---
    // Creamos el árbol con el mapa de frecuencias calculado
    HuffmanTree arbol(frecuencias);
    
    // Obtenemos el diccionario { "14f": "101", "2a": "0", ... }
    std::map<std::string, std::string> diccionarioCodigos = arbol.getCodes();

    // --- PASO 3: Reconstruir y Guardar en TXT ---
    
    // PRECAUCIÓN: Si la matriz es GIGANTE, crear este vector puede llenar la RAM.
    // Si la matriz es pequeña/mediana, este método es el más sencillo y claro.
    
    // 3a. Obtener el código del fondo ("2a")
    std::string codigoFondo = diccionarioCodigos[valorMasFrecuente];
    
    // 3b. Crear matriz en memoria llena con el código de fondo
    std::vector<std::vector<std::string>> matrizReconstruida(
        totalFilas, 
        std::vector<std::string>(totalCols, codigoFondo)
    );

    // 3c. Sobrescribir solo las posiciones que venían en la lista dispersa
    for (const auto& item : datosDispersos) {
        if (item.fila < totalFilas && item.col < totalCols) {
            // Buscamos el código de este valor específico y lo colocamos
            matrizReconstruida[item.fila][item.col] = diccionarioCodigos[item.valor];
        }
    }

    // 3d. Escribir al archivo
    std::ofstream archivo(nombreArchivoSalida);
    if (archivo.is_open()) {
        for (int i = 0; i < totalFilas; ++i) {
            for (int j = 0; j < totalCols; ++j) {
                archivo << matrizReconstruida[i][j] << " ";
            }
            archivo << "\n"; 
        }
        archivo.close();
        // Opcional: Mensaje de debug
        // std::cout << "Archivo generado: " << nombreArchivoSalida << "\n";
    } else {
        std::cerr << "Error: No se pudo crear el archivo " << nombreArchivoSalida << "\n";
    }

    return diccionarioCodigos;
}

} // namespace huffman