#pragma once

#include <string>
#include <vector>
#include <memory>
#include "model/TranslationUnit.h"

// --- Ocultación de la API de C ---
// Declaramos por adelantado los tipos opacos de libclang.
typedef struct CXIndexImpl* CXIndex;
typedef struct CXTranslationUnitImpl* CXTranslationUnit;
// Usamos una definición de puntero vacío para el cursor
typedef struct CXCursor { void* _data[3]; } CXCursor; 
enum CXChildVisitResult; // Declaración por adelantado de la enumeración

namespace cppuml {
namespace parser {

/**
 * @class LibClangParser
 * @brief Un adaptador que envuelve la API C de libclang.
 *
 * Esta clase inicializa el 'CXIndex' de clang y proporciona una única
 * función 'parse' que toma un archivo fuente y devuelve nuestro
 * modelo de datos interno ('TranslationUnit').
 *
 */
class LibClangParser {
public:
    /**
     * @brief Inicializa el índice de libclang.
     */
    LibClangParser();

    /**
     * @brief Libera los recursos de libclang.
     */
    ~LibClangParser();

    // Deshabilitar copia y movimiento (el CXIndex no es copiable)
    LibClangParser(const LibClangParser&) = delete;
    LibClangParser& operator=(const LibClangParser&) = delete;

    /**
     * @brief Analiza un único archivo fuente de C++.
     *
     * Punto de entrada principal. Orquesta el análisis de
     * libclang y la visita del AST.
     *
     *
     * @param sourceFile La ruta al archivo .cpp o .h a analizar.
     * @param compileArgs Una lista de argumentos del compilador (ej. "-I/include").
     * @return Un puntero único a nuestro modelo de TranslationUnit poblado.
     */
    std::unique_ptr<model::TranslationUnit> parse(
        const std::string& sourceFile,
        const std::vector<std::string>& compileArgs = {});

private:
    /**
     * @brief El 'callback' visitante de libclang.
     *
     * Función estática que se llama por cada nodo en el AST.
     * Redirige al visitante C++orientado a objetos ('AstVisitor').
     *
     * @param client_data Un puntero a nuestro objeto 'AstVisitor'.
     */
    static CXChildVisitResult visitor(CXCursor cursor, CXCursor parent, void* client_data);

    /**
     * @brief El índice principal de libclang, inicializado en el constructor.
     */
    CXIndex m_index;
};

} // namespace parser
} // namespace cppuml