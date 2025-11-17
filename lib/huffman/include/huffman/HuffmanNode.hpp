//HuffmanNode.hpp
#pragma once

#include <memory>   // Para std::shared_ptr
#include <string>   // <-- AÑADIDO: Para poder usar std::string

namespace huffman {

/**
 * @class HuffmanNode
 * @brief Representa un solo nodo en el árbol de Huffman.
 * Puede ser un nodo hoja (con un símbolo) o un nodo interno (con hijos).
 */
class HuffmanNode {
public:
    // --- Tipos de datos actualizados ---
    // 1. Usamos 'std::string' para el símbolo (ej: "2f", "4h").
    // 2. Usamos 'int' para la frecuencia.
    // 3. Usamos std::shared_ptr para gestionar la memoria de los hijos.
    // ------------------------------------

    std::string data;
    int frequency;

    // Punteros a los nodos hijos
    std::shared_ptr<HuffmanNode> left;
    std::shared_ptr<HuffmanNode> right;

    /**
     * @brief Constructor para un nodo HOJA.
     * @param data El símbolo (token) que representa este nodo.
     * @param frequency La frecuencia de aparición de este símbolo.
     */
    // Nota: Usamos "const std::string&" para pasar el string de
    // forma eficiente, sin crear copias innecesarias.
    HuffmanNode(const std::string& data, int frequency);

    /**
     * @brief Constructor para un nodo INTERNO.
     * @param left El hijo izquierdo.
     * @param right El hijo derecho.
     */
    HuffmanNode(std::shared_ptr<HuffmanNode> left, std::shared_ptr<HuffmanNode> right);

    /**
     * @brief Comprueba si este nodo es una hoja.
     * @return true si ambos hijos son nulos (nullptr), false en caso contrario.
     */
    bool isLeaf() const;

}; // fin de la clase HuffmanNode

} // fin del namespace huffman