#ifndef LECTOR_HPP
#define LECTOR_HPP

#include <vector>
#include <string>
#include <cstdint>


struct UTF_8Text {
    std::string utf8;
    std::vector<uint32_t> codepoints;
    
    // Métodos útiles para manejar el texto carácter por carácter
    size_t length() const { return codepoints.size(); }
    bool empty() const { return codepoints.empty(); }
    void clear() { utf8.clear(); codepoints.clear(); }
    std::pair<uint32_t, int> analizarFrecuencia(const std::vector<uint32_t>& codepoints);
    std::vector<uint32_t> simplificarCodepoints(const std::vector<uint32_t>& codepoints);
    std::pair<uint32_t, int> analizarFrecuenciaSimplificada(const std::vector<uint32_t>& codepoints);

};

namespace text {
    std::vector<unsigned char> leerBytes(const std::string& ruta);
    std::vector<std::string> almacenarPorLineas(const UTF_8Text& texto);
    std::pair<size_t, size_t> determinarDimensionesMatrizUnicode(const std::vector<std::string>& lineas);
    bool tieneBOM_UTF8(const std::vector<unsigned char>& b);
    bool tieneBOM_UTF16LE(const std::vector<unsigned char>& b);
    bool tieneBOM_UTF16BE(const std::vector<unsigned char>& b);
    void push_utf8(std::string& out, uint32_t cp);
    bool utf8_to_codepoints(const std::string& s, std::vector<uint32_t>& cps);
    void latin1_to_utf8(const std::vector<unsigned char>& bytes, std::string& out);
    bool utf16_to_codepoints(const std::vector<unsigned char>& b, bool big_endian,size_t offset, std::vector<uint32_t>& cps);

};

class Normalizer {
public:
    static UTF_8Text cargar_normalizado_UTF8(const std::string& ruta);
    static std::tuple<int, int, std::vector<std::vector<int>>> CrearEntregarMatriz(UTF_8Text data);
    static void mostrarLetrasYPosiciones(UTF_8Text data);
};

#endif