#ifndef CPP_UML_GENERATOR_CORE_MODEL_METHOD_H
#define CPP_UML_GENERATOR_CORE_MODEL_METHOD_H

#include <string>
#include <vector>
#include <memory>  // Para std::unique_ptr
#include <utility> // Para std::move

#include "Element.h"
#include "Type.h"
#include "Field.h" // Usado para los parámetros

namespace cppuml {

/**
 * @brief Modela una función miembro (método) dentro de una Clase o Struct.
 *
 * Hereda de Element, tiene un Type de retorno, y posee una lista de
 * parámetros (modelados como objetos Field).
 */
class Method : public Element {
public:
    /**
     * @brief Constructor para un Method.
     * @param name El nombre del método (p.ej., "calculateSum").
     * @param returnType El objeto Type del valor de retorno (p.ej., "void", "int").
     */
    Method(std::string name, Type returnType)
        : Element(std::move(name)),
          m_returnType(std::move(returnType)),
          m_isStatic(false),
          m_isConst(false),
          m_isVirtual(false),
          m_isPureVirtual(false) {}

    /**
     * @brief Destructor virtual por defecto.
     */
    ~Method() override = default;

    // --- Implementación de la Interfaz Element ---

    /**
     * @brief Obtiene el tipo de este elemento.
     * @return ElementKind::Method.
     */
    ElementKind getKind() const override { return ElementKind::Method; }

    // --- Acceso Público ---

    /**
     * @brief Obtiene el tipo de retorno del método.
     */
    const Type& getReturnType() const { return m_returnType; }

    /**
     * @brief Establece el tipo de retorno del método.
     */
    void setReturnType(Type type) { m_returnType = std::move(type); }

    // --- Gestión de Parámetros (Lineamiento P2) ---

    /**
     * @brief Añade un parámetro a este método.
     * El método toma posesión del parámetro.
     * @param param Un unique_ptr a un objeto Field que modela el parámetro.
     */
    void addParameter(std::unique_ptr<Field> param) {
        m_parameters.push_back(std::move(param));
    }

    /**
     * @brief Obtiene una referencia constante a la lista de parámetros.
     */
    const std::vector<std::unique_ptr<Field>>& getParameters() const {
        return m_parameters;
    }

    // --- Modificadores de Método ---

    void setStatic(bool val = true) { m_isStatic = val; }
    bool isStatic() const { return m_isStatic; }

    void setConst(bool val = true) { m_isConst = val; }
    bool isConst() const { return m_isConst; }

    void setVirtual(bool val = true) { m_isVirtual = val; }
    bool isVirtual() const { return m_isVirtual; }

    void setPureVirtual(bool val = true) {
        m_isPureVirtual = val;
        if (val) m_isVirtual = true; // Un método puro es implícitamente virtual
    }
    bool isPureVirtual() const { return m_isPureVirtual; }

private:
    Type m_returnType;
    std::vector<std::unique_ptr<Field>> m_parameters;

    bool m_isStatic;
    bool m_isConst;
    bool m_isVirtual;
    bool m_isPureVirtual;
};

} // namespace cppuml

#endif // CPP_UML_GENERATOR_CORE_MODEL_METHOD_H