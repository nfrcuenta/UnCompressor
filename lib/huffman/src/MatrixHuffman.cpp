#include "huffman/MatrixHuffman.hpp"
#include "huffman/HuffmanTree.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstring>

namespace huffman {

// --- CLASE AYUDANTE PARA ESCRIBIR BITS ---
class BitWriter {
    std::ofstream& out;
    unsigned char buffer; 
    int bitIndex;         

public:
    BitWriter(std::ofstream& stream) : out(stream), buffer(0), bitIndex(0) {}

    void writeBits(const std::string& bits) {
        for (char c : bits) {
            if (c == '1') {
                buffer |= (1 << (7 - bitIndex)); 
            }
            bitIndex++;

            if (bitIndex == 8) {
                out.put(buffer);
                buffer = 0;
                bitIndex = 0;
            }
        }
    }

    void flush() {
        if (bitIndex > 0) {
            out.put(buffer);
        }
    }
};

void escribirInt(std::ofstream& out, int valor) {
    out.write(reinterpret_cast<const char*>(&valor), sizeof(valor));
}

void escribirString(std::ofstream& out, const std::string& s) {
    int len = s.length();
    escribirInt(out, len);
    out.write(s.c_str(), len);
}

// --- DECLARACIÓN DE FUNCIÓN INTERNA ---
void exportarBinario(
    const std::string& nombreArchivo,
    int filas,
    int cols,
    const std::string& valorFondo,
    const std::vector<Triplete>& tripletas,
    const std::map<std::string, std::string>& codigos
);


// --- FUNCIÓN PRINCIPAL ---
std::map<std::string, std::string> procesarMatrizYExportar(
    const std::vector<Triplete>& datosDispersos,
    const std::string& valorMasFrecuente,
    int totalFilas,
    int totalCols,
    const std::string& nombreArchivoSalida)
{
    // 1. Calcular Frecuencias
    std::map<std::string, int> frecuencias;
    for (const auto& item : datosDispersos) {
        frecuencias[item.valor]++;
    }
    
    long long totalCeldas = (long long)totalFilas * totalCols;
    long long celdasVacias = totalCeldas - datosDispersos.size();
    if (celdasVacias > 0) {
        frecuencias[valorMasFrecuente] += celdasVacias;
    }

    // 2. Generar Huffman
    HuffmanTree arbol(frecuencias);
    auto diccionario = arbol.getCodes();

    // 3. Exportar
    if (nombreArchivoSalida.find(".bin") != std::string::npos) {
        exportarBinario(nombreArchivoSalida, totalFilas, totalCols, valorMasFrecuente, datosDispersos, diccionario);
    } else {
        // Modo Texto (simplificado)
        std::string codigoFondo = diccionario[valorMasFrecuente];
        std::vector<std::vector<std::string>> matrizReconstruida(totalFilas, std::vector<std::string>(totalCols, codigoFondo));
        
        for (const auto& item : datosDispersos) {
            if(item.fila < totalFilas && item.col < totalCols)
                matrizReconstruida[item.fila][item.col] = diccionario[item.valor];
        }
        
        std::ofstream archivo(nombreArchivoSalida);
        for (int i = 0; i < totalFilas; ++i) {
            for (int j = 0; j < totalCols; ++j) {
                archivo << matrizReconstruida[i][j]; 
            }
            archivo << "\n";
        }
    }

    return diccionario;
}

// --- IMPLEMENTACIÓN BINARIA ---
void exportarBinario(
    const std::string& nombreArchivo,
    int filas,
    int cols,
    const std::string& valorFondo,
    const std::vector<Triplete>& tripletas,
    const std::map<std::string, std::string>& codigos)
{
    std::ofstream out(nombreArchivo, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "Error al crear archivo binario.\n";
        return;
    }

    // A. CABECERA
    escribirInt(out, filas);
    escribirInt(out, cols);
    
    // B. DICCIONARIO
    int tamDiccionario = codigos.size();
    escribirInt(out, tamDiccionario);
    for (const auto& par : codigos) {
        escribirString(out, par.first);  
        escribirString(out, par.second); 
    }

    // C. RECONSTRUCCIÓN EN MEMORIA (Para bitstream continuo)
    std::string codigoFondo = codigos.at(valorFondo);
    std::vector<std::vector<std::string>> matriz(filas, std::vector<std::string>(cols, codigoFondo));

    for (const auto& tri : tripletas) {
        if (tri.fila < filas && tri.col < cols) {
            matriz[tri.fila][tri.col] = codigos.at(tri.valor);
        }
    }

    // D. ESCRIBIR BITS
    BitWriter bitWriter(out);
    
    for (int i = 0; i < filas; ++i) {
        for (int j = 0; j < cols; ++j) {
            bitWriter.writeBits(matriz[i][j]);
        }
    }
    
    bitWriter.flush(); 
    out.close();
    std::cout << "[BINARIO] Archivo optimizado generado: " << nombreArchivo << "\n";
}

} // namespace huffman
