#ifndef CPP_UML_GENERATOR_CORE_MODEL_TYPE_H
#define CPP_UML_GENERATOR_CORE_MODEL_TYPE_H

#include <string>
#include <vector>
#include <utility> // Para std::move
#include <sstream> // Para getFullName

#include "Element.h" // Requerido para Element*

namespace cppuml {

// Declaración anticipada para la relación de amistad 
class LibclangParser; 

/**
 * @brief Modela un tipo de C++ de forma semántica.
 *
 * Es un "Objeto de Valor": es copiable y no hereda de Element.
 * Un Field o Method contendrá un Type por valor.
 */
class Type {
public:
    /**
     * @brief Constructor para un tipo simple.
     * @param name El nombre base del tipo (p.ej., "int", "vector", "MyClass").
     */
    explicit Type(std::string name)
        : m_name(std::move(name)), m_customTypeElement(nullptr),
          m_isConst(false), m_isVolatile(false), 
          m_isPointer(false), m_isReference(false) {}

    // --- Semántica de Copia y Movimiento ---
    
    /**
     * Los tipos son copiables y movibles por defecto.
     */
    Type(const Type&) = default;
    Type& operator=(const Type&) = default;
    Type(Type&&) = default;
    Type& operator=(Type&&) = default;

    // --- Modificadores de Tipo ---

    void setConst(bool val = true) { m_isConst = val; }
    bool isConst() const { return m_isConst; }

    void setVolatile(bool val = true) { m_isVolatile = val; }
    bool isVolatile() const { return m_isVolatile; }

    void setPointer(bool val = true) { m_isPointer = val; }
    bool isPointer() const { return m_isPointer; }

    void setReference(bool val = true) { m_isReference = val; }
    bool isReference() const { return m_isReference; }

    // --- Enlace del Modelo ---

    /**
     * @brief Establece el enlace a la definición del elemento (si es un tipo de usuario).
     * @param element Puntero no propietario al Class/Struct/Enum que define este tipo.
     */
    void setCustomTypeElement(Element* element) { m_customTypeElement = element; }

    /**
     * @brief Obtiene el enlace a la definición del elemento.
     * @return Puntero no propietario, o nullptr si es un tipo primitivo.
     */
    Element* getCustomTypeElement() const { return m_customTypeElement; }

    // --- Plantillas ---

    /**
     * @brief Añade un parámetro de plantilla a este tipo.
     * @param param El tipo del parámetro de plantilla.
     */
    void addTemplateParameter(Type param) {
        m_templateParameters.push_back(std::move(param));
    }

    const std::vector<Type>& getTemplateParameters() const {
        return m_templateParameters;
    }

    // --- Acceso y Utilidades ---

    const std::string& getName() const { return m_name; }

    /**
     * @brief Reconstruye la cadena completa del tipo (para exportadores y depuración).
     * @return Una cadena formateada, p.ej., "const MyClass*&" o "std::vector<int>".
     */
    std::string getFullName() const {
        std::stringstream ss;
        if (m_isConst) ss << "const ";
        if (m_isVolatile) ss << "volatile ";
        
        ss << m_name;

        if (!m_templateParameters.empty()) {
            ss << "<";
            for (size_t i = 0; i < m_templateParameters.size(); ++i) {
                ss << m_templateParameters[i].getFullName();
                if (i < m_templateParameters.size() - 1) ss << ", ";
            }
            ss << ">";
        }

        if (m_isPointer) ss << "*";
        if (m_isReference) ss << "&";

        return ss.str();
    }

private:
    std::string m_name;
    Element* m_customTypeElement; // Puntero no propietario

    std::vector<Type> m_templateParameters;

    // Calificadores y modificadores
    bool m_isConst;
    bool m_isVolatile;
    bool m_isPointer;
    bool m_isReference;

    // Amistad: Opcionalmente, permite al parser acceder a los miembros
    // privados para construir el tipo de manera eficiente.
    // friend class LibclangParser; 
};

} // namespace cppuml

#endif // CPP_UML_GENERATOR_CORE_MODEL_TYPE_H