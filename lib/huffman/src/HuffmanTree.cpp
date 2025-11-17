//HuffmanTree.cpp
// Incluimos la cabecera que estamos implementando
#include "huffman/HuffmanTree.hpp"

// Aunque la mayoría ya están en el .hpp, incluimos las
// librerías que usamos explícitamente en este .cpp
#include <queue>    // Para std::priority_queue
#include <vector>   // Para el contenedor de la priority_queue
#include <memory>   // Para std::make_shared

namespace huffman {

/**
 * @brief Constructor de HuffmanTree.
 */
HuffmanTree::HuffmanTree(const std::map<std::string, int>& frequencies)
    : root(nullptr) // Inicializamos la raíz como nula
{
    // Si el mapa de frecuencias está vacío, no hay nada que hacer.
    if (frequencies.empty()) {
        return;
    }

    // 1. Construir el árbol
    buildTree(frequencies);

    // 2. Generar los códigos
    
    // CASO ESPECIAL: Si solo hay un símbolo (el árbol es solo un nodo hoja).
    // Asignamos '0' como su código por convención.
    if (root != nullptr && root->isLeaf()) {
        huffmanCodes[root->data] = "0";
    }
    // Caso normal: El árbol tiene más de un nodo
    else {
        // Empezamos la recursión para generar códigos desde la raíz
        generateCodes(root, "");
    }
}

/**
 * @brief Obtiene el mapa de códigos de Huffman (getter).
 */
const std::map<std::string, std::string>& HuffmanTree::getCodes() const
{
    // Simplemente devuelve el mapa que ya fue llenado.
    return huffmanCodes;
}

// --- Métodos Privados ---

/**
 * @brief Construye el árbol de Huffman usando una cola de prioridad.
 */
void HuffmanTree::buildTree(const std::map<std::string, int>& frequencies)
{
    // 1. Crear la cola de prioridad (min-heap).
    // Esta cola ordenará los nodos usando nuestro 'CompareNode'
    // para que los nodos con MENOR frecuencia salgan primero.
    std::priority_queue<std::shared_ptr<HuffmanNode>,
                        std::vector<std::shared_ptr<HuffmanNode>>,
                        CompareNode> pq;

    // 2. Poblar la cola de prioridad con los nodos hoja.
    // Iteramos sobre el mapa de frecuencias que recibimos.
    for (const auto& pair : frequencies) {
        // 'pair.first' es el std::string (símbolo, ej: "2f")
        // 'pair.second' es el int (frecuencia)
        
        // Creamos un nuevo nodo hoja y lo añadimos a la cola.
        // Usamos std::make_shared para crear el puntero inteligente.
        pq.push(std::make_shared<HuffmanNode>(pair.first, pair.second));
    }

    // 3. Construir el árbol.
    // El bucle se ejecuta mientras haya más de un nodo en la cola.
    while (pq.size() > 1) {
        // 3a. Sacar los dos nodos con menor frecuencia
        std::shared_ptr<HuffmanNode> left = pq.top();
        pq.pop();

        std::shared_ptr<HuffmanNode> right = pq.top();
        pq.pop();

        // 3b. Crear un nuevo nodo INTERNO con estos dos como hijos.
        // Nuestro constructor de HuffmanNode se encarga de sumar
        // las frecuencias (left->frequency + right->frequency).
        auto parent = std::make_shared<HuffmanNode>(left, right);

        // 3c. Añadir el nuevo nodo padre de vuelta a la cola.
        // La cola lo reordenará según su nueva frecuencia (la suma).
        pq.push(parent);
    }

    // 4. Fin del bucle.
    // Si la cola no estaba vacía, el único nodo que queda
    // es el nodo raíz de todo el árbol.
    if (!pq.empty()) {
        root = pq.top();
    }
}

/**
 * @brief Función recursiva (DFS) para generar los códigos binarios.
 */
void HuffmanTree::generateCodes(std::shared_ptr<HuffmanNode> node, const std::string& currentCode)
{
    // Caso base de seguridad: si el nodo es nulo, no hacer nada.
    if (node == nullptr) {
        return;
    }

    // Caso base de la recursión: ¡Es un nodo hoja!
    if (node->isLeaf()) {
        // Hemos llegado a un símbolo. Guardamos el código binario
        // acumulado (currentCode) en nuestro mapa de resultados.
        huffmanCodes[node->data] = currentCode;
        return; // Terminamos esta rama de la recursión.
    }

    // Paso recursivo: Este es un nodo interno (no es una hoja).
    
    // Ir a la izquierda, añadiendo un '0' al código actual.
    generateCodes(node->left, currentCode + "0");

    // Ir a la derecha, añadiendo un '1' al código actual.
    generateCodes(node->right, currentCode + "1");
}

} // fin del namespace huffman