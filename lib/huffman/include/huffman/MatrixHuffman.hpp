#ifndef MATRIX_HUFFMAN_HPP
#define MATRIX_HUFFMAN_HPP

#include <string>
#include <vector>
#include <map>

namespace huffman {

struct Triplete {
    int fila;
    int col;
    std::string valor;
};

// Función principal que decide si exportar a TXT o BIN
std::map<std::string, std::string> procesarMatrizYExportar(
    const std::vector<Triplete>& datosDispersos,
    const std::string& valorMasFrecuente,
    int totalFilas,
    int totalCols,
    const std::string& nombreArchivoSalida
);

// Función interna para la lógica binaria
void exportarBinario(
    const std::string& nombreArchivo,
    int filas,
    int cols,
    const std::string& valorFondo,
    const std::vector<Triplete>& tripletas,
    const std::map<std::string, std::string>& codigos
);

} // namespace huffman

#endif // MATRIX_HUFFMAN_HPP
