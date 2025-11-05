#include "libclang_parser.h"
// Se incluye SÓLO en el archivo .cpp
#include <clang-c/Index.h>

// --- Registros / Utilidades ---
#include <iostream>
#include <string>
#include <stack> // <--- Necesario para el estado

// --- Inclusiones del Modelo ---
#include "model/Class.h"
#include "model/Method.h"
#include "model/Field.h"
#include "model/Namespace.h"

namespace cppuml {
namespace parser {

// --- Utilidad de Ayuda ---
// (Sin cambios)
static std::string cx_to_std(CXString cx) {
    const char* c_str = clang_getCString(cx);
    if (!c_str) {
        return "";
    }
    std::string str(c_str);
    clang_disposeString(cx);
    return str;
}

// --- Clase Visitante de AST (NUEVA) ---

/**
 * @class AstVisitor
 * @brief Objeto de estado C++ que implementa la lógica de visita.
 *
 * La API C de libclang requiere un callback estático ('visitor').
 * Ese callback simplemente redirigirá a una instancia de esta clase.
 * Esta clase mantiene el estado (pila de namespaces, clase actual)
 * mientras se recorre el AST.
 */
class AstVisitor {
public:
    /**
     * @brief Construye el visitante.
     * @param tu Puntero al modelo de TranslationUnit que se está poblando.
     */
    explicit AstVisitor(model::TranslationUnit* tu) : m_tu(tu) {}

    /**
     * @brief El método de visita real que contiene la lógica.
     * * Esta función es llamada por el "trampolín" estático.
     */
    CXChildVisitResult visitNode(CXCursor cursor, CXCursor parent) {
        // 1. Obtener información básica del nodo
        CXCursorKind kind = clang_getCursorKind(cursor);
        std::string name = cx_to_std(clang_getCursorSpelling(cursor));
        
        // (Opcional: depuración)
        // int depth = m_namespaceStack.size();
        // std::cout << std::string(depth * 2, ' ') 
        //           << "[Visit] Kind: " << clang_getCursorKindSpelling(kind)
        //           << ", Name: " << name << "\n";

        // 2. Lógica del Visitante basada en el estado
        switch (kind) {
            
            case CXCursor_Namespace: {
                auto ns = std::make_unique<model::Namespace>(name);
                model::Namespace* nsPtr = ns.get();

                // Añadir al padre (ya sea la TU o el namespace en la cima de la pila)
                if (m_namespaceStack.empty()) {
                    m_tu->addNamespace(std::move(ns));
                } else {
                    m_namespaceStack.top()->addNamespace(std::move(ns));
                }

                // --- Manejo de Estado y Recursión ---
                m_namespaceStack.push(nsPtr); // PUSH
                // Recurrimos manualmente
                clang_visitChildren(cursor, LibClangParser::visitor, this); 
                m_namespaceStack.pop(); // POP
                // Le decimos a libclang que no vuelva a recurrir (ya lo hicimos)
                return CXChildVisit_Continue;
            }

            case CXCursor_ClassDecl:
            case CXCursor_StructDecl: {
                // TODO: Manejar declaraciones adelantadas (forward declarations)
                // if (clang_isCursorDefinition(cursor) == 0) { ... }
                
                auto newClass = std::make_unique<model::Class>(name);
                // TODO: newClass->setKind(kind == CXCursor_StructDecl ? ...);
                model::Class* classPtr = newClass.get();

                // Añadir al padre (ya sea la TU o el namespace en la cima de la pila)
                if (m_namespaceStack.empty()) {
                    m_tu->addClass(std::move(newClass));
                } else {
                    m_namespaceStack.top()->addClass(std::move(newClass));
                }
                
                // --- Manejo de Estado y Recursión ---
                // Guardar el estado de la clase padre (para clases anidadas)
                model::Class* stashedParentClass = m_currentClass; 
                m_currentClass = classPtr; // SET
                
                clang_visitChildren(cursor, LibClangParser::visitor, this); 
                
                m_currentClass = stashedParentClass; // RESET
                return CXChildVisit_Continue;
            }

            case CXCursor_FieldDecl: {
                if (m_currentClass) {
                    auto newField = std::make_unique<model::Field>(name);
                    // TODO: Obtener visibilidad (clang_getCXXAccessSpecifier)
                    // TODO: Obtener tipo (clang_getCursorType)
                    m_currentClass->addField(std::move(newField));
                }
                // (Nodo hoja, no hay recursión)
                return CXChildVisit_Continue;
            }

            case CXCursor_CXXMethod: {
                if (m_currentClass) {
                    auto newMethod = std::make_unique<model::Method>(name);
                    // TODO: Obtener visibilidad, parámetros, tipo de retorno, etc.
                    m_currentClass->addMethod(std::move(newMethod));
                }
                // (Nodo hoja, no hay recursión)
                return CXChildVisit_Continue;
            }
            
            case CXCursor_CXXBaseSpecifier: {
                 if (m_currentClass) {
                    // Este nodo representa 'public BaseClass'
                    // El *nombre* está en el tipo
                    std::string baseName = cx_to_std(clang_getCursorTypeName(clang_getCursorType(cursor)));
                    // TODO: Añadir relación de herencia al modelo
                    // m_currentClass->addBaseClass(baseName, clang_getCXXAccessSpecifier(cursor));
                    std::cout << "DEBUG: Clase " << m_currentClass->m_name << " hereda de " << baseName << std::endl;
                 }
                return CXChildVisit_Continue;
            }

            default:
                // No estamos manejando este nodo, pero queremos
                // seguir visitando a sus hijos (ej. TranslationUnit)
                return CXChildVisit_Recurse;
        }
    }

private:
    model::TranslationUnit* m_tu;
    std::stack<model::Namespace*> m_namespaceStack;
    model::Class* m_currentClass = nullptr;
};


// --- Constructor / Destructor ---
// (Sin cambios)

LibClangParser::LibClangParser() {
    m_index = clang_createIndex(0, 1);
}

LibClangParser::~LibClangParser() {
    clang_disposeIndex(m_index);
}


// --- Método de Análisis Principal (MODIFICADO) ---

std::unique_ptr<model::TranslationUnit> LibClangParser::parse(
    const std::string& sourceFile,
    const std::vector<std::string>& compileArgs) {

    // 1. Crear el objeto de modelo raíz
    auto tuModel = std::make_unique<model::TranslationUnit>();
    tuModel->m_name = sourceFile;

    // 2. Convertir argumentos
    std::vector<const char*> cArgs;
    cArgs.reserve(compileArgs.size());
    for (const auto& arg : compileArgs) {
        cArgs.push_back(arg.c_str());
    }

    // 3. Analizar el archivo
    CXTranslationUnit tu = clang_parseTranslationUnit(
        m_index,
        sourceFile.c_str(),
        cArgs.data(), static_cast<int>(cArgs.size()),
        nullptr, 0,
        CXTranslationUnit_None
    );

    if (!tu) {
        std::cerr << "Error: No se pudo analizar (parse) " << sourceFile << std::endl;
        return nullptr;
    }

    // 4. Obtener el cursor raíz
    CXCursor rootCursor = clang_getTranslationUnitCursor(tu);

    // 5. *** CAMBIO PRINCIPAL ***
    // Crear nuestro objeto visitante C++ con estado
    AstVisitor visitorContext(tuModel.get());

    // Iniciar la visita recursiva.
    // Pasamos un puntero a nuestro 'visitorContext' como 'client_data'.
    clang_visitChildren(rootCursor, visitor, &visitorContext);

    // 6. Liberar la unidad de traducción
    clang_disposeTranslationUnit(tu);

    return tuModel;
}


// --- VISITOR (Trampolín) (MODIFICADO) ---

CXChildVisitResult LibClangParser::visitor(CXCursor cursor, CXCursor parent, void* client_data) {
    
    // 1. Re-castear el 'client_data' a nuestro objeto visitante C++
    AstVisitor* context = static_cast<AstVisitor*>(client_data);

    // 2. Delegar la llamada al método de miembro (que tiene estado)
    return context->visitNode(cursor, parent);
}


} // namespace parser
} // namespace cppuml