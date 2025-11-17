// 1. Incluimos las cabeceras necesarias
#include "huffman/HuffmanNode.hpp"

// std::string y std::shared_ptr se incluyen a través de HuffmanNode.hpp,
// pero es buena práctica incluir lo que usas (aunque no es estrictamente
// necesario aquí).
#include <string>

// Abrimos el mismo espacio de nombres
namespace huffman {

/**
 * @brief Constructor para un nodo HOJA (con std::string).
 */
// Usamos la lista de inicialización para asignar los valores.
HuffmanNode::HuffmanNode(const std::string& data, int frequency)
    : data(data),          // Asigna el string del símbolo
      frequency(frequency),  // Asigna la frecuencia
      left(nullptr),       // Las hojas no tienen hijos
      right(nullptr)
{
    // El cuerpo del constructor está vacío.
}

/**
 * @brief Constructor para un nodo INTERNO.
 */
// Este constructor no cambia, ya que solo depende de sus hijos.
HuffmanNode::HuffmanNode(std::shared_ptr<HuffmanNode> left, std::shared_ptr<HuffmanNode> right)
    : data(""),            // Un nodo interno no tiene símbolo. Usamos un string vacío.
      frequency(left->frequency + right->frequency), // La frecuencia es la suma de los hijos.
      left(left),          // Asigna el hijo izquierdo
      right(right)         // Asigna el hijo derecho
{
    // El cuerpo también está vacío.
}

/**
 * @brief Comprueba si este nodo es una hoja.
 */
// Esta función tampoco cambia.
bool HuffmanNode::isLeaf() const
{
    // Un nodo es una hoja si no tiene hijos.
    return left == nullptr && right == nullptr;
}

} // fin del namespace huffman