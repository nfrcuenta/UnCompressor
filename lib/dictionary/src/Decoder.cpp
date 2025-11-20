#include "dictionary/Decoder.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <cstdint>

using namespace dictionary;

namespace {

// Encapsula la cabecera estándar del .bin (dimensiones y tamaño del diccionario).
struct BinaryHeader {
    int rows = 0;
    int cols = 0;
    int dictSize = 0;
};

// Lee un entero de 32 bits del stream y valida que exista suficiente data.
int readInt(std::ifstream& in) {
    int value = 0;
    if (!in.read(reinterpret_cast<char*>(&value), sizeof(value))) {
        throw std::runtime_error("Archivo .bin incompleto al leer enteros.");
    }
    return value;
}

// Cada string se codifica como: <int longitud><bytes>. Esta función lo reconstruye.
std::string readString(std::ifstream& in) {
    int len = readInt(in);
    if (len < 0) {
        throw std::runtime_error("Longitud negativa detectada en cadena del diccionario.");
    }
    std::string s(static_cast<size_t>(len), '\0');
    if (len > 0 && !in.read(&s[0], len)) {
        throw std::runtime_error("Archivo .bin incompleto al leer cadena del diccionario.");
    }
    return s;
}

// Lector de bits perezoso: amortiza lecturas byte a byte del payload.
class BitReader {
public:
    explicit BitReader(std::ifstream& stream) : in(stream), bitIndex(8), buffer(0) {}

    // Devuelve 0/1 o -1 en EOF
    int readBit() {
        if (bitIndex == 8) {
            if (!in.read(reinterpret_cast<char*>(&buffer), 1)) {
                return -1;
            }
            bitIndex = 0;
        }
        int bit = (buffer >> (7 - bitIndex)) & 1;
        ++bitIndex;
        return bit;
    }

private:
    std::ifstream& in;
    int bitIndex;
    unsigned char buffer;
};

// Convierte un codepoint Unicode a UTF-8 y lo concatena al string de salida.
void appendUtf8(std::string& out, uint32_t cp) {
    if (cp <= 0x7F) {
        out.push_back(static_cast<char>(cp));
    } else if (cp <= 0x7FF) {
        out.push_back(static_cast<char>(0xC0 | (cp >> 6)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else if (cp <= 0xFFFF) {
        out.push_back(static_cast<char>(0xE0 | (cp >> 12)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else if (cp <= 0x10FFFF) {
        out.push_back(static_cast<char>(0xF0 | (cp >> 18)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else {
        throw std::runtime_error("Codepoint fuera de rango UTF-8.");
    }
}

// Los símbolos almacenados en el diccionario son enteros en texto (codepoints).
// Esta función los traduce nuevamente a caracteres UTF-8.
std::string tokenToUtf8(const std::string& token) {
    if (token.empty()) {
        return "";
    }

    size_t consumed = 0;
    long long value = 0;
    try {
        value = std::stoll(token, &consumed, 10);
    } catch (const std::exception&) {
        throw std::runtime_error("Simbolo no numerico encontrado durante la decodificacion.");
    }

    if (consumed != token.size()) {
        throw std::runtime_error("Simbolo con caracteres adicionales detectado durante la decodificacion.");
    }
    if (value < 0 || value > 0x10FFFF) {
        throw std::runtime_error("Codepoint fuera de rango en el payload decodificado.");
    }

    std::string utf8;
    appendUtf8(utf8, static_cast<uint32_t>(value));
    return utf8;
}

// Reconstruye la matriz textual en formato legible (filas separadas por '\n').
std::string formatMatrix(const std::vector<std::string>& tokens, int rows, int cols) {
    if (rows == 0 || cols == 0) {
        return "";
    }
    std::ostringstream oss;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            size_t idx = static_cast<size_t>(i) * static_cast<size_t>(cols) + static_cast<size_t>(j);
            if (idx >= tokens.size()) {
                throw std::runtime_error("Cantidad de tokens insuficiente para reconstruir la matriz.");
            }
            oss << tokenToUtf8(tokens[idx]);
        }
        if (i + 1 < rows) {
            oss << '\n';
        }
    }
    return oss.str();
}

// Extrae filas/columnas y rellena el Dictionary con los pares almacenados.
BinaryHeader readHeaderAndDictionary(std::ifstream& in, Dictionary& dict) {
    BinaryHeader header;
    header.rows = readInt(in);
    header.cols = readInt(in);
    header.dictSize = readInt(in);

    if (header.dictSize <= 0) {
        throw std::runtime_error("Diccionario vacío o inválido en el binario.");
    }

    dict.clear();
    for (int i = 0; i < header.dictSize; ++i) {
        auto symbol = readString(in);
        auto code = readString(in);
        if (code.empty()) {
            throw std::runtime_error("Código vacío encontrado en el diccionario del binario.");
        }
        dict.insert(code, symbol);
    }
    return header;
}

} // namespace

// Punto de entrada público: abre el .bin, carga el diccionario y decodifica el payload.
std::string Decoder::decodeFile(const std::string& path, Dictionary& dict) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("No se pudo abrir el archivo binario.");

    BinaryHeader header = readHeaderAndDictionary(file, dict);
    long long totalCells = static_cast<long long>(header.rows) * static_cast<long long>(header.cols);
    if (totalCells < 0) {
        throw std::runtime_error("Dimensiones inválidas en el binario.");
    }

    BitReader bitReader(file);
    std::vector<std::string> tokens;
    tokens.reserve(static_cast<size_t>(totalCells));
    std::string current_code;
    current_code.reserve(32);

    while (static_cast<long long>(tokens.size()) < totalCells) {
        int bit = bitReader.readBit();
        if (bit == -1) {
            throw std::runtime_error("Archivo binario incompleto al leer payload.");
        }
        current_code.push_back(bit ? '1' : '0');

        std::string decoded;
        if (dict.tryGetSymbol(current_code, decoded)) {
            tokens.push_back(decoded);
            current_code.clear();
        } else if (!dict.isValidPrefix(current_code)) {
            throw std::runtime_error("Código Huffman inválido en el payload del binario.");
        }
    }

    return formatMatrix(tokens, header.rows, header.cols);
}

void Decoder::writeText(const std::string& path, const std::string& text) {
    std::ofstream out(path);
    if (!out.is_open())
        throw std::runtime_error("No se pudo crear archivo de salida.");

    out << text;
    out.close();
}
