#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace dictionary {

// Diccionario ligero: guarda pares codigo-binario -> simbolo decoded (como string).
// Mantiene tambien los prefijos para validar mientras leemos bit a bit.
class Dictionary {
private:
    std::unordered_map<std::string, std::string> codes_;      // cod completo -> simbolo(s)
    std::unordered_set<std::string> prefixes_;         // prefijos validos

public:
    Dictionary();
    ~Dictionary();

    // Inserta o actualiza un codigo Huffman con su simbolo asociado.
    void insert(const std::string& code, const std::string& symbol);
    // Busca el simbolo para un codigo completo; retorna false si no existe.
    bool tryGetSymbol(const std::string& code, std::string& symbol) const;
    // Indica si existe algun codigo que comience con este prefijo.
    bool isValidPrefix(const std::string& prefix) const;
    // Limpia cualquier contenido previo antes de volver a cargar desde un binario.
    void clear();
};

} // namespace dictionary
