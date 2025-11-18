#include "dictionary/Dictionary.hpp"
#include <stdexcept>

using namespace dictionary;

Dictionary::Dictionary() = default;
Dictionary::~Dictionary() = default;

// Inserta un par código->símbolo validando previamente que el código sea binario.
void Dictionary::insert(const std::string& code, const std::string& symbol) {
    if (code.empty()) {
        throw std::invalid_argument("Código Huffman vacío.");
    }

    // Validamos que el código solo contenga bits; si hubiera otro símbolo
    // el decoder terminaría en un estado inconsistente, así que lo frenamos aquí.
    for (char bit : code) {
        if (bit != '0' && bit != '1') {
            throw std::invalid_argument("Código contiene caracteres que no son 0 o 1.");
        }
    }

    // Guardamos/actualizamos el mapeo exacto.
    codes_[code] = symbol;

    // Para decodificar incrementalmente necesitamos saber qué prefijos son válidos.
    // Insertamos todos los prefijos propios de este código en el set.
    for (size_t i = 1; i < code.size(); ++i) {
        prefixes_.insert(code.substr(0, i));
    }
}

// Busca el símbolo asociado; retorna false si el código aún no es completo.
bool Dictionary::tryGetSymbol(const std::string& code, std::string& symbol) const {
    auto it = codes_.find(code);
    if (it == codes_.end()) {
        return false;
    }
    symbol = it->second;
    return true;
}

// Verifica si existe algún código almacenado que empiece con este prefijo.
bool Dictionary::isValidPrefix(const std::string& prefix) const {
    return prefixes_.find(prefix) != prefixes_.end();
}

// Restablece el diccionario a un estado vacío (útil antes de cargar un binario).
void Dictionary::clear() {
    codes_.clear();
    prefixes_.clear();
}
