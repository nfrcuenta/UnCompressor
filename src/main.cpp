#include "lector.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>

int main() {
    std::string ruta;
    
    // Pedir archivo al usuario
    std::cout << "Ingrese la ruta del archivo de texto: ";
    std::getline(std::cin, ruta);
    
    if (ruta.empty()) {
        ruta = "texto.txt"; // Valor por defecto
    }
    
    // Verificar si el archivo existe
    std::ifstream test(ruta);
    if (!test.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo '" << ruta << "'\n";
        std::cerr << "Creando archivo de ejemplo...\n";
        
        std::ofstream archivo_prueba(ruta);
        if (archivo_prueba.is_open()) {
            archivo_prueba << "Texto de ejemplo con café y mañana.\n";
            archivo_prueba << "Incluye caracteres especiales: áéíóú ñ Ñ ¿? ¡!\n";
            archivo_prueba << "Y números: 12345";
            archivo_prueba.close();
            std::cout << "Archivo '" << ruta << "' creado con texto de ejemplo.\n";
        } else {
            std::cerr << "No se pudo crear el archivo.\n";
            return 1;
        }
    } else {
        test.close();
    }
    

    // Resto del código: cargar y procesar usando el objeto retornado
    UTF_8Text t = Normalizer::cargar_normalizado_UTF8(ruta);

    if (t.utf8.empty() && t.codepoints.empty()) {
        std::cerr << "Fallo al cargar/normalizar.\n";
        return 1;
    }

    

    // Simplificar los codepoints eliminando ceros
    auto codepoints_simplificados = t.simplificarCodepoints(t.codepoints);

    // Analizar frecuencia en los datos simplificados
    auto [codepoint_mas_frecuente, frecuencia] = t.analizarFrecuencia(codepoints_simplificados);
    


    //Realizar matriz con simplificacion basica
    auto resultado = Normalizer::CrearEntregarMatriz(t);
    auto filas = std::get<0>(resultado);
    auto columnas = std::get<1>(resultado);
    auto matriz = std::get<2>(resultado);
   
    // ... mostrar resultados
    return 0;
}