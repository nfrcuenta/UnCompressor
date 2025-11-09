#include "lector.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <map>
#include <algorithm>
#include <sstream>
#include <tuple>

// ========== UTILIDADES DE LECTURA DE ARCHIVOS ==========

/**
 * Lee un archivo completo y lo carga en un vector de bytes
 * @param ruta: Ruta del archivo a leer
 * @return Vector con todos los bytes del archivo, o vector vacío si hay error
 */
std::vector<unsigned char> text::leerBytes(const std::string& ruta) {
    // Abrir archivo en modo binario
    std::ifstream f(ruta, std::ios::binary);
    if (!f) {
        std::cerr << "No se pudo abrir el archivo: " << ruta << "\n";
        return {};
    }
    
    // Verificar tamaño del archivo moviendo el puntero al final
    f.seekg(0, std::ios::end);
    std::streamsize size = f.tellg();
    f.seekg(0, std::ios::beg);
    
    if (size == 0) {
        std::cerr << "Archivo vacío: " << ruta << "\n";
        return {};
    }
    
    // Leer todos los bytes usando iteradores
    return { std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>() };
}

/** Almacenamiento de texto por lineas */
std::vector<std::string> text::almacenarPorLineas(const UTF_8Text& texto) {
    std::vector<std::string> lineas;
    
    if (texto.utf8.empty()) {
        return lineas; // Texto vacío
    }
    
    std::istringstream stream(texto.utf8);
    std::string linea;
    
    while (std::getline(stream, linea)) {
        // Eliminar retornos de carro si existen (caso Windows)
        if (!linea.empty() && linea.back() == '\r') {
            linea.pop_back();
        }
        lineas.push_back(linea);
    }
    
    return lineas;
}

// Versión que cuenta codepoints Unicode en lugar de bytes
std::pair<size_t, size_t> text::determinarDimensionesMatrizUnicode(const std::vector<std::string>& lineas) {
    if (lineas.empty()) return {0, 0};
    
    size_t filas = lineas.size();
    size_t max_columnas = 0;
    
    for (const auto& linea : lineas) {
        std::vector<uint32_t> codepoints;
        if (text::utf8_to_codepoints(linea, codepoints)) {
            if (codepoints.size() > max_columnas) {
                max_columnas = codepoints.size();
            }
        }
    }
    
    return {filas, max_columnas};
}

/**
 * Detecta si el archivo tiene BOM (Byte Order Mark) de UTF-8
 * El BOM de UTF-8 es: EF BB BF
 */
bool text::tieneBOM_UTF8(const std::vector<unsigned char>& b) {
    return b.size() >= 3 && b[0] == 0xEF && b[1] == 0xBB && b[2] == 0xBF;
}

/**
 * Detecta si el archivo tiene BOM de UTF-16 Little Endian
 * El BOM de UTF-16 LE es: FF FE
 */
bool text::tieneBOM_UTF16LE(const std::vector<unsigned char>& b) {
    return b.size() >= 2 && b[0] == 0xFF && b[1] == 0xFE;
}

/**
 * Detecta si el archivo tiene BOM de UTF-16 Big Endian  
 * El BOM de UTF-16 BE es: FE FF
 */
bool text::tieneBOM_UTF16BE(const std::vector<unsigned char>& b) {
    return b.size() >= 2 && b[0] == 0xFE && b[1] == 0xFF;
}

// ========== CODIFICACIÓN Y DECODIFICACIÓN UNICODE ==========

/**
 * Convierte un punto de código Unicode a una secuencia UTF-8 y la añade a un string
 * @param out: String donde se añadirá la secuencia UTF-8
 * @param cp: Punto de código Unicode (0 a 0x10FFFF)
 * 
 * ESQUEMA DE CODIFICACIÓN UTF-8:
 * - 1 byte:  0xxxxxxx (ASCII: 0-127)
 * - 2 bytes: 110xxxxx 10xxxxxx (hasta 2047)
 * - 3 bytes: 1110xxxx 10xxxxxx 10xxxxxx (hasta 65535)
 * - 4 bytes: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx (hasta 1,114,111)
 */
void text::push_utf8(std::string& out, uint32_t cp) {
    if (cp <= 0x7F) {
        // Caracter ASCII - 1 byte
        out.push_back(static_cast<char>(cp));
    } else if (cp <= 0x7FF) {
        // 2 bytes en UTF-8
        out.push_back(static_cast<char>(0xC0 | (cp >> 6)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else if (cp <= 0xFFFF) {
        // 3 bytes en UTF-8
        out.push_back(static_cast<char>(0xE0 | (cp >> 12)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else if (cp <= 0x10FFFF) {
        // 4 bytes en UTF-8
        out.push_back(static_cast<char>(0xF0 | (cp >> 18)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else {
        throw std::invalid_argument("Codepoint fuera de rango UTF-8");
    }
}

/**
 * Decodifica una cadena UTF-8 y extrae los puntos de código Unicode individuales
 * @param s: Cadena en formato UTF-8
 * @param cps: Vector donde se almacenarán los puntos de código resultantes
 * @return true si la decodificación fue exitosa, false si hay errores
 */
bool text::utf8_to_codepoints(const std::string& s, std::vector<uint32_t>& cps) {
    cps.clear();
    size_t i = 0, n = s.size();
    
    while (i < n) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        uint32_t cp = 0;
        size_t extra = 0; // Número de bytes adicionales esperados

        // Determinar el patrón del primer byte y cuántos bytes adicionales esperar
        if (c <= 0x7F) {
            // ASCII - 1 byte
            cp = c;
            extra = 0;
        } else if ((c & 0xE0) == 0xC0) {
            // 2 bytes - patrón: 110xxxxx
            cp = c & 0x1F;
            extra = 1;
            if (cp < 0x02) return false; // Detectar "overlong encoding"
        } else if ((c & 0xF0) == 0xE0) {
            // 3 bytes - patrón: 1110xxxx
            cp = c & 0x0F;
            extra = 2;
        } else if ((c & 0xF8) == 0xF0) {
            // 4 bytes - patrón: 11110xxx
            cp = c & 0x07;
            extra = 3;
            if (cp > 0x04) return false; // Fuera de rango Unicode
        } else {
            return false; // Byte inicial inválido
        }

        // Verificar que hay suficientes bytes en la cadena
        if (i + extra >= n) return false;
        
        // Procesar bytes de continuación (deben tener patrón 10xxxxxx)
        for (size_t j = 1; j <= extra; ++j) {
            unsigned char cc = static_cast<unsigned char>(s[i + j]);
            if ((cc & 0xC0) != 0x80) return false; // Byte de continuación inválido
            cp = (cp << 6) | (cc & 0x3F);
        }

        // Validaciones de seguridad Unicode
        if (cp >= 0xD800 && cp <= 0xDFFF) return false; // Surrogates no permitidos en UTF-8
        if (cp > 0x10FFFF) return false; // Fuera de rango Unicode máximo
        
        // Detectar "overlong encoding" (usar más bytes de los necesarios)
        if (extra == 1 && cp < 0x80) return false;
        if (extra == 2 && cp < 0x800) return false;
        if (extra == 3 && cp < 0x10000) return false;

        cps.push_back(cp);
        i += 1 + extra; // Avanzar al siguiente caracter
    }
    return true;
}

/**
 * Convierte texto Latin-1 (ISO-8859-1) a UTF-8
 * Latin-1 usa 1 byte por caracter y cubre los caracteres europeos occidentales
 */
void text::latin1_to_utf8(const std::vector<unsigned char>& bytes, std::string& out) {
    out.clear();
    out.reserve(bytes.size() * 2); // Reservar espacio para posible expansión
    
    for (unsigned char b : bytes) {
        if (b < 0x80) {
            // Caracteres ASCII (0-127) permanecen igual
            out.push_back(static_cast<char>(b));
        } else {
            // Caracteres Latin-1 (128-255) se convierten a 2 bytes UTF-8
            out.push_back(static_cast<char>(0xC0 | (b >> 6)));
            out.push_back(static_cast<char>(0x80 | (b & 0x3F)));
        }
    }
}

/**
 * Decodifica texto UTF-16 a puntos de código Unicode
 * @param b: Vector de bytes en UTF-16
 * @param big_endian: true si es big-endian, false si es little-endian
 * @param offset: Posición donde empezar a decodificar (después del BOM)
 * @param cps: Vector donde almacenar los puntos de código resultantes
 * @return true si la decodificación fue exitosa
 * 
 * UTF-16 usa 2 bytes para caracteres básicos y 4 bytes para caracteres especiales (surrogate pairs)
 */
bool text::utf16_to_codepoints(const std::vector<unsigned char>& b, bool big_endian,
                                size_t offset, std::vector<uint32_t>& cps) {
    cps.clear();
    
    // UTF-16 siempre usa pares de bytes, verificar que la longitud sea par
    if ((b.size() - offset) % 2 != 0) return false;
    
    for (size_t i = offset; i + 1 < b.size(); i += 2) {
        // Leer un código de 16 bits (2 bytes) considerando el endianness
        uint16_t u = big_endian 
            ? (static_cast<uint16_t>(b[i]) << 8 | static_cast<uint16_t>(b[i+1]))
            : (static_cast<uint16_t>(b[i+1]) << 8 | static_cast<uint16_t>(b[i]));
            
        if (u >= 0xD800 && u <= 0xDBFF) {
            // High surrogate - necesita un low surrogate para formar un caracter especial
            if (i + 3 >= b.size()) return false; // Verificar que hay bytes suficientes
            
            // Leer el low surrogate
            uint16_t v = big_endian
                ? (static_cast<uint16_t>(b[i+2]) << 8 | static_cast<uint16_t>(b[i+3]))
                : (static_cast<uint16_t>(b[i+3]) << 8 | static_cast<uint16_t>(b[i+2]));
                
            if (v < 0xDC00 || v > 0xDFFF) return false; // Low surrogate inválido
            
            // Calcular el punto de código completo a partir del par de surrogates
            // Fórmula: 0x10000 + ((high - 0xD800) << 10) + (low - 0xDC00)
            uint32_t cp = 0x10000 + (((u - 0xD800) << 10) | (v - 0xDC00));
            cps.push_back(cp);
            i += 2; // Saltar el low surrogate que ya procesamos
        } else if (u >= 0xDC00 && u <= 0xDFFF) {
            return false; // Low surrogate sin high surrogate - error
        } else {
            // Caracter básico de 16 bits (sin surrogate)
            cps.push_back(u);
        }
    }
    return true;
}

// Función para analizar frecuencia de codepoints
std::pair<uint32_t, int> UTF_8Text::analizarFrecuencia(const std::vector<uint32_t>& codepoints) {
    if (codepoints.empty()) {
        return {0, 0}; // Vector vacío
    }
    
    std::map<uint32_t, int> frecuencia;
    
    // Contar frecuencia de cada codepoint
    for (uint32_t cp : codepoints) {
        frecuencia[cp]++;
    }
    
    // Encontrar el elemento con máxima frecuencia
    auto max_element = std::max_element(
        frecuencia.begin(), 
        frecuencia.end(),
        [](const auto& a, const auto& b) { 
            return a.second < b.second; 
        }
    );
    
    return *max_element;
}

/**
 * Simplifica el vector de codepoints eliminando los valores cero
 * y conservando solo los datos significativos
 * @param codepoints: Vector original con todos los codepoints
 * @return Vector simplificado sin ceros
 */
std::vector<uint32_t> UTF_8Text::simplificarCodepoints(const std::vector<uint32_t>& codepoints) {
    std::vector<uint32_t> simplificado;
    simplificado.reserve(codepoints.size()); // Reservar espacio para mejor rendimiento
    
    for (uint32_t cp : codepoints) {
        if (cp != 0) {
            simplificado.push_back(cp);
        }
    }
    
    // Reducir capacidad para ahorrar memoria si hay muchos ceros
    simplificado.shrink_to_fit();
    return simplificado;
}

std::pair<uint32_t, int> UTF_8Text::analizarFrecuenciaSimplificada(const std::vector<uint32_t>& codepoints) {
    auto simplificado = simplificarCodepoints(codepoints);
    return analizarFrecuencia(simplificado);
}

// ========== FUNCIÓN PRINCIPAL DE NORMALIZACIÓN ==========

/**
 * Función principal que carga un archivo de texto y lo normaliza a UTF-8
 * Detecta automáticamente la codificación y convierte a UTF-8 + vector de codepoints
 * @param ruta: Ruta del archivo a cargar
 * @return Estructura UTF_8Text con el texto normalizado
 * 
 * ESTRATEGIA DE DETECCIÓN:
 * 1. UTF-16 con BOM (más fácil de detectar)
 * 2. UTF-8 (con o sin BOM)  
 * 3. Fallback a Latin-1 (ISO-8859-1)
 */
UTF_8Text Normalizer::cargar_normalizado_UTF8(const std::string& ruta) {
    UTF_8Text out;
    
    try {
        // 1. Leer todos los bytes del archivo
        auto bytes = text::leerBytes(ruta);
        if (bytes.empty()) {
            return out; // Archivo vacío o error de lectura
        }

        // 2. PRIMERO: Detectar y procesar UTF-16 con BOM
        // Los BOM de UTF-16 son muy específicos y fáciles de detectar
        if (text::tieneBOM_UTF16LE(bytes) || text::tieneBOM_UTF16BE(bytes)) {
            bool be = text::tieneBOM_UTF16BE(bytes); // Determinar endianness
            std::vector<uint32_t> cps;

            if (!text::utf16_to_codepoints(bytes, be, /*offset=*/2, cps)) {
                std::cerr << "Error decodificando UTF-16 en: " << ruta << "\n";
                return out;
            }
            
            out.codepoints = std::move(cps);
            // Reconstruir string UTF-8 desde los puntos de código
            out.utf8.reserve(out.codepoints.size() * 4); // Reservar espacio máximo
            for (uint32_t cp : out.codepoints) {
                text::push_utf8(out.utf8, cp);
            }
            return out;
        }

        // 3. SEGUNDO: Intentar como UTF-8 (con o sin BOM)
        size_t offset = text::tieneBOM_UTF8(bytes) ? 3 : 0;
        std::string as_utf8(bytes.begin() + offset, bytes.end());
        std::vector<uint32_t> cps;
        
        // Intentar decodificar como UTF-8 válido
        if (text::utf8_to_codepoints(as_utf8, cps)) {
            out.utf8 = std::move(as_utf8);
            out.codepoints = std::move(cps);
            return out;
        }

        // 4. TERCERO: Fallback a Latin-1 (compatibilidad con texto europeo antiguo)
        std::string utf8_from_l1;
        text::latin1_to_utf8(bytes, utf8_from_l1);
        if (text::utf8_to_codepoints(utf8_from_l1, cps)) {
            out.utf8 = std::move(utf8_from_l1);
            out.codepoints = std::move(cps);
            return out;
        }

        std::cerr << "No se pudo determinar la codificación del archivo: " << ruta << "\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Excepción al procesar " << ruta << ": " << e.what() << "\n";
    }
    
    
    return out;
}
std::tuple<int, int, std::vector<std::vector<int>>> Normalizer::CrearEntregarMatriz(UTF_8Text data) {
    std::vector<std::vector<int>> matriz;
    auto lineas = text::almacenarPorLineas(data);
    auto [filas, columnas] = text::determinarDimensionesMatrizUnicode(lineas);
    auto MasFrecuente = data.analizarFrecuenciaSimplificada(data.codepoints);
    
    if (lineas.empty()) {
        return {0, 0, matriz};
    }
    
    for (size_t i = 0; i < lineas.size(); ++i) {
        std::vector<uint32_t> cps_linea;
        if (text::utf8_to_codepoints(lineas[i], cps_linea)) {
            for (size_t j = 0; j < cps_linea.size(); ++j) {
                uint32_t cp = cps_linea[j];
                // Solo agregar si no es cero y no es el más frecuente
                if (cp != 0 && cp != MasFrecuente.first) {
                    matriz.push_back({
                        static_cast<int>(i), 
                        static_cast<int>(j), 
                        static_cast<int>(cp)
                    });
                }
            }
        }
    }

    return  std::make_tuple(filas, columnas, matriz);
}
