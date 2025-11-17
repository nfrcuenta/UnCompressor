//HuffmanTree.hpp
#pragma once

// Incluimos nuestro Nodo, ya que el Árbol está compuesto por Nodos.
#include "huffman/HuffmanNode.hpp"

// --- Librerías estándar que necesitaremos ---

#include <memory>   // Para std::shared_ptr
#include <string>   // Para std::string
#include <map>      // Para el mapa de frecuencias y el mapa de códigos
#include <vector>   // Requerido por std::priority_queue
#include <queue>    // Para std::priority_queue (la clave del algoritmo)


namespace huffman {

// --- Ayudante: Comparador para la Cola de Prioridad ---

/**
 * @struct CompareNode
 * @brief Objeto de función para ordenar la cola de prioridad (min-heap).
 *
 * std::priority_queue por defecto es un "max-heap" (ordena de mayor a menor).
 * Para Huffman, necesitamos un "min-heap" (ordenar de menor a mayor frecuencia).
 *
 * Este struct le dice a la cola de prioridad que considere "mayor"
 * al nodo que tenga la frecuencia más alta.
 */
struct CompareNode {
    // Sobrecarga del operador ()
    bool operator()(const std::shared_ptr<HuffmanNode>& a, const std::shared_ptr<HuffmanNode>& b) const {
        // Devuelve true si la frecuencia de 'a' es MAYOR que la de 'b'.
        // Esto hace que los nodos con MENOR frecuencia tengan prioridad.
        return a->frequency > b->frequency;
    }
};


// --- Definición de la Clase Principal ---

/**
 * @class HuffmanTree
 * @brief Gestiona la construcción del árbol de Huffman y la
 * generación de los códigos binarios para cada símbolo.
 */
class HuffmanTree {
public:
    /**
     * @brief Constructor.
     * Construye el árbol de Huffman inmediatamente al ser creado.
     * @param frequencies Un mapa donde la clave es el símbolo (string)
     * y el valor es su frecuencia (int).
     */
    HuffmanTree(const std::map<std::string, int>& frequencies);

    /**
     * @brief Obtiene el mapa de códigos de Huffman generados.
     * @return Un mapa constante donde la clave es el símbolo (ej: "2f")
     * y el valor es su código binario (ej: "011").
     */
    const std::map<std::string, std::string>& getCodes() const;

private:
    // El nodo raíz de nuestro árbol.
    std::shared_ptr<HuffmanNode> root;

    // Mapa para almacenar los códigos generados (Ej: "2f" -> "01")
    std::map<std::string, std::string> huffmanCodes;

    /**
     * @brief Función privada que construye el árbol.
     * Es llamada por el constructor.
     * @param frequencies El mapa de frecuencias.
     */
    void buildTree(const std::map<std::string, int>& frequencies);

    /**
     * @brief Función recursiva privada para generar los códigos.
     * Recorre el árbol (DFS) y almacena los códigos en 'huffmanCodes'.
     * @param node El nodo actual que se está visitando.
     * @param currentCode El código binario acumulado hasta este nodo.
     */
    void generateCodes(std::shared_ptr<HuffmanNode> node, const std::string& currentCode);

}; // fin de la clase HuffmanTree

} // fin del namespace huffman