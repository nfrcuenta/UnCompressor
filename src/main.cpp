#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>
#include <tuple>
#include <cctype>

#include "huffman/MatrixHuffman.hpp"
#include "lector.hpp"
#include "dictionary/Dictionary.hpp"
#include "dictionary/Decoder.hpp"

using dictionary::Decoder;
using dictionary::Dictionary;

static int run_compression();
static int run_decompression();
static void print_usage();

static int run_compression() {
    // =========================================================
    // PASO 1: INTERACCIÓN BÁSICA (Pedir archivo)
    // =========================================================
    std::string ruta;
    std::cout << "=== COMPRESOR HUFFMAN PARA MATRICES DISPERSAS ===\n";
    std::cout << "Ingrese la ruta del archivo de texto a procesar: ";
    std::getline(std::cin, ruta);
    
    if (ruta.empty()) {
        ruta = "texto.txt"; // Valor por defecto
    }

    // Verificar si existe, si no, crear uno de prueba
    std::ifstream checkFile(ruta);
    if (!checkFile.is_open()) {
        std::cerr << "[AVISO] No se encontro '" << ruta << "'. Creando archivo de prueba...\n";
        std::ofstream dummy(ruta);
        if (dummy.is_open()) {
            dummy << "Texto de ejemplo para probar la matriz dispersa.\n";
            dummy << "Repetimos palabras para generar frecuencia: ejemplo ejemplo.\n";
            dummy.close();
        } else {
            std::cerr << "[ERROR] No se pudo crear el archivo.\n";
            return 1;
        }
    } else {
        checkFile.close();
    }

    // =========================================================
    // PASO 2: NORMALIZACIÓN (Usando libreria 'lector')
    // =========================================================
    std::cout << "[INFO] Cargando y normalizando texto...\n";
    
    UTF_8Text t = Normalizer::cargar_normalizado_UTF8(ruta);
    
    if (t.utf8.empty() && t.codepoints.empty()) {
        std::cerr << "[ERROR] Fallo al cargar o normalizar el texto.\n";
        return 1;
    }

    // Análisis previo necesario para obtener el dato más frecuente
    auto codepoints_simplificados = t.simplificarCodepoints(t.codepoints);
    auto [codepoint_mas_frecuente, frecuencia] = t.analizarFrecuencia(codepoints_simplificados);

    // Obtener la estructura de matriz desde el Normalizer
    auto resultado = Normalizer::CrearEntregarMatriz(t);
    
    int filas = std::get<0>(resultado);
    int cols  = std::get<1>(resultado);
    auto& datos_origen = std::get<2>(resultado);

    std::cout << "[INFO] Matriz generada: " << filas << "x" << cols << " con " 
              << datos_origen.size() << " elementos no vacios.\n";

    // =========================================================
    // PASO 3: INTEGRACIÓN CON HUFFMAN (Tu librería)
    // =========================================================
    std::cout << "--- Iniciando Codificacion Huffman ---\n";

    // A. Definir el valor de fondo
    std::string valorFondoStr = std::to_string(codepoint_mas_frecuente);
    std::cout << "Valor de fondo (mas frecuente): '" << valorFondoStr << "'\n";

    // B. Adaptar los datos
    std::vector<huffman::Triplete> entradaHuffman;
    entradaHuffman.reserve(datos_origen.size());

    for (const auto& item : datos_origen) {
        // Como 'item' es un vector<int>, usamos corchetes [], NO std::get
        int f = item[0]; 
        int c = item[1]; 
        
        // Convertimos el valor (int) a string para Huffman
        std::string val = std::to_string(item[2]); 

        // Filtramos el fondo
        if (val != valorFondoStr) {
            entradaHuffman.push_back({f, c, val});
        }
    }

    std::cout << "[INFO] Elementos dispersos finales a comprimir: " << entradaHuffman.size() << "\n";

    // C. Ejecutar la compresión y exportación
    // IMPORTANTE: Usamos extensión .bin para activar el modo binario
    std::string archivoSalida = "matriz_comprimida.bin";
    
    auto diccionario = huffman::procesarMatrizYExportar(
        entradaHuffman,
        valorFondoStr,
        filas,
        cols,
        archivoSalida
    );

    // =========================================================
    // PASO 4: REPORTE FINAL
    // =========================================================
    std::cout << "\n=== PROCESO TERMINADO CON EXITO ===\n";
    std::cout << "1. Archivo generado: " << archivoSalida << "\n";
    std::cout << "2. Simbolos unicos en diccionario: " << diccionario.size() << "\n";
    
    if (diccionario.find(valorFondoStr) != diccionario.end()) {
        std::cout << "3. Codigo asignado al fondo ('" << valorFondoStr << "'): " << diccionario[valorFondoStr] << "\n";
    } else {
        std::cout << "3. El fondo fue optimizado (no presente en diccionario explicito).\n";
    }

    return 0;
}

static int run_decompression() {
    std::cout << "=== DECODIFICADOR HUFFMAN PARA MATRICES DISPERSAS ===\n";
    std::cout << "Ingrese la ruta del archivo .bin a decodificar [matriz_comprimida.bin]: ";
    std::string bin_path;
    std::getline(std::cin, bin_path);
    if (bin_path.empty()) {
        bin_path = "matriz_comprimida.bin";
    }

    if (!std::filesystem::exists(bin_path)) {
        std::cerr << "[ERROR] No se encontro el archivo: " << bin_path << "\n";
        return 1;
    }

    std::cout << "Ingrese la ruta del archivo de texto donde guardar la salida [matriz_recuperada.txt]: ";
    std::string output_path;
    std::getline(std::cin, output_path);
    if (output_path.empty()) {
        output_path = "matriz_recuperada.txt";
    }

    Dictionary dict;

    try {
        std::cout << "[INFO] Decodificando '" << bin_path << "'...\n";
        std::string decoded = Decoder::decodeFile(bin_path, dict);
        Decoder::writeText(output_path, decoded);
        std::cout << "[INFO] Matriz restaurada y guardada en '" << output_path << "'.\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] No se pudo decodificar: " << e.what() << "\n";
        return 1;
    }
}

static void print_usage() {
	std::cout << "Usage:\n";
	std::cout << "  Normal mode: run without arguments and follow prompts (process text normalization)\n";
	std::cout << "  Decode mode:\n";
	std::cout << "    ./uncompressor decode <input.bin> <output.txt>\n";
}

int main(int argc, char** argv) {
	if (argc > 1 && std::string(argv[1]) == "decode") {
		if (argc < 4) {
			print_usage();
			return 1;
		}
		std::string input_bin = argv[2];
		std::string output_txt = argv[3];

		Dictionary dict;

		try {
			std::string decoded = Decoder::decodeFile(input_bin, dict);
			Decoder::writeText(output_txt, decoded);
			std::error_code ec;
			if (std::filesystem::remove(input_bin, ec)) {
				std::cout << "Binario temporal eliminado: " << input_bin << "\n";
			} else if (ec) {
				std::cerr << "Advertencia: no se pudo eliminar " << input_bin << ": " << ec.message() << "\n";
			}
			std::cout << "Decodificación completa. Salida: " << output_txt << "\n";
			return 0;
		} catch (const std::exception& e) {
			std::cerr << "Error en decodificación: " << e.what() << "\n";
			return 1;
		}
	}

	std::cout << "Seleccione una opcion:\n";
	std::cout << "  1) Comprimir texto\n";
	std::cout << "  2) Descomprimir binario\n";
	std::cout << "Opcion [1]: ";
	std::string choice;
	std::getline(std::cin, choice);
	char normalized = choice.empty() ? '1' : static_cast<char>(std::tolower(choice.front()));

	if (normalized == '2' || normalized == 'd') {
		return run_decompression();
	}

	return run_compression();
}
