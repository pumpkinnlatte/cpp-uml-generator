#ifndef CPP_UML_GENERATOR_CORE_MODEL_RELATIONSHIP_H
#define CPP_UML_GENERATOR_CORE_MODEL_RELATIONSHIP_H

#include "Element.h"

namespace cppuml {

/**
 * @brief Define el tipo de relación UML.
 */
enum class RelationshipKind {
    Relationship,
    Inheritance, // Aunque se almacena en Class, podría ser explícita aquí
    Association,
    Composition,
    Aggregation,
    Usage
};

/**
 * @brief Clase base abstracta para todas las relaciones UML.
 *
 *
 * No hereda de Element, ya que una Relación no es un Elemento del
 * código C++ per se, sino una conexión entre ellos.
 */
class Relationship {
public:
    /**
     * @brief Destructor virtual por defecto.
     */
    virtual ~Relationship() = default;

    // --- Semántica de Copia y Movimiento ---
    Relationship(const Relationship&) = delete;
    Relationship& operator=(const Relationship&) = delete;
    Relationship(Relationship&&) = default;
    Relationship& operator=(Relationship&&) = default;

    // --- Interfaz Abstracta ---

    /**
     * @brief Obtiene el tipo de esta relación (p.ej., Asociación).
     */
    virtual RelationshipKind getKind() const = 0;

    /**
     * @brief Obtiene el elemento de origen de la relación.
     * @return Puntero no propietario al elemento de origen.
     */
    virtual Element* getSource() const = 0;

    /**
     * @brief Obtiene el elemento de destino de la relación.
     * @return Puntero no propietario al elemento de destino.
     */
    virtual Element* getDestination() const = 0;

protected:
    // El constructor protegido asegura que solo las clases derivadas
    // puedan ser instanciadas.
    Relationship() = default;
};

} // namespace cppuml

#endif // CPP_UML_GENERATOR_CORE_MODEL_RELATIONSHIP_H