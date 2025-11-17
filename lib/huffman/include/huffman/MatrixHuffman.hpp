//MatrixHuffman.hpp
#ifndef MATRIX_HUFFMAN_HPP
#define MATRIX_HUFFMAN_HPP

#include <string>
#include <vector>
#include <map>

namespace huffman {

// Estructura para recibir los datos de la matriz dispersa
struct Triplete {
    int fila;
    int col;
    std::string valor;
};

/**
 * @brief Procesa una matriz dispersa, genera códigos Huffman y exporta a TXT.
 * * @param datosDispersos Vector con los tripletas (fila, col, valor) no nulos.
 * @param valorMasFrecuente El string que representa el fondo (ej: "2a").
 * @param totalFilas Dimensiones originales de la matriz.
 * @param totalCols Dimensiones originales de la matriz.
 * @param nombreArchivoSalida Ruta donde se guardará la matriz codificada.
 * @return std::map<std::string, std::string> El diccionario generado.
 */
std::map<std::string, std::string> procesarMatrizYExportar(
    const std::vector<Triplete>& datosDispersos,
    const std::string& valorMasFrecuente,
    int totalFilas,
    int totalCols,
    const std::string& nombreArchivoSalida
);

} // namespace huffman

#endif // MATRIX_HUFFMAN_HPP