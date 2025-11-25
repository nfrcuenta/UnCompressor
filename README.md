# UnCompressor

UnCompressor es una herramienta escrita en **C++** que permite **comprimir y descomprimir archivos de texto (.txt)** utilizando el algoritmo de **Huffman**.  
El objetivo principal del proyecto es reducir el tamaño de archivos de texto generando una codificación óptima basada en la frecuencia de los caracteres.

## Características principales

- **Compresión**
  - Analiza el archivo de entrada.
  - Calcula la frecuencia de cada carácter.
  - Construye un árbol de Huffman y genera los códigos.
  - Produce un archivo comprimido `.bin`.

- **Descompresión**
  - Lee el archivo binario.
  - Reconstruye el diccionario Huffman.
  - Decodifica los bits y recupera el archivo `.txt` original.

## Estructura del proyecto

- `huffman/` – Construcción del árbol y generación de códigos.
- `dictionary/` – Manejo del diccionario Huffman (código → carácter).
- `lector/` – Lectura de archivos y conteo de frecuencias.

## Objetivo

Ofrecer una implementación clara, didáctica y funcional utilizando matrices dispersas, diccionarios y arboles, aplicando lo aprendido en 
estructura de datos

## Documentación

- [Manual de usuario](docs/Dpcumentacion.pdf)
- [Presentación del proyecto](docs/Presentacion.pdf)

