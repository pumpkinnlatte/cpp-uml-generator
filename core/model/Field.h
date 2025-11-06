#ifndef CPP_UML_GENERATOR_CORE_MODEL_FIELD_H
#define CPP_UML_GENERATOR_CORE_MODEL_FIELD_H

#include <string>
#include <utility> // Para std::move

#include "Element.h"
#include "Type.h"

namespace cppuml {

/**
 * @brief Modela un miembro de datos dentro de una Clase o Struct.
 *
 * Hereda de Element y contiene un Type.
 */
class Field : public Element {
public:
    /**
     * @brief Constructor para un Field.
     * @param name El nombre del campo (p.ej., "m_count").
     * @param type El objeto Type que describe este campo (p.ej., "int", "std::string").
     */
    Field(std::string name, Type type)
        : Element(std::move(name)), m_type(std::move(type)), m_isStatic(false) {}

    /**
     * @brief Destructor virtual por defecto.
     */
    ~Field() override = default;

    // --- Implementación de la Interfaz Element ---

    /**
     * @brief Obtiene el tipo de este elemento.
     * @return ElementKind::Field.
     */
    ElementKind getKind() const override { return ElementKind::Field; }

    // --- Acceso Público ---

    /**
     * @brief Obtiene el tipo del campo.
     */
    const Type& getType() const { return m_type; }

    /**
     * @brief Establece el tipo del campo.
     */
    void setType(Type type) { m_type = std::move(type); }

    /**
     * @brief Establece si el campo es estático.
     */
    void setStatic(bool isStatic) { m_isStatic = isStatic; }

    /**
     * @brief Verifica si el campo es estático.
     */
    bool isStatic() const { return m_isStatic; }

private:
    Type m_type;
    bool m_isStatic;
};

} // namespace cppuml

#endif // CPP_UML_GENERATOR_CORE_MODEL_FIELD_H