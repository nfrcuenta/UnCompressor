#pragma once
#include <string>
#include "Dictionary.hpp"

namespace dictionary {

class Decoder {
public:
    // Lee binario que incluye cabecera+diccionario+payload y regresa matriz textual.
    static std::string decodeFile(const std::string& path, Dictionary& dict);

    // Guardar resultado en archivo
    static void writeText(const std::string& path, const std::string& text);
};

}
