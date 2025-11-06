#ifndef CPP_UML_GENERATOR_CORE_MODEL_CLASS_H
#define CPP_UML_GENERATOR_CORE_MODEL_CLASS_H

#include <string>
#include <vector>
#include <memory>  // Para std::unique_ptr
#include <utility> // Para std::move

#include "Element.h"
#include "Field.h"
#include "Method.h"

// Se necesita una declaración anticipada de Class para el puntero
// en InheritanceInfo.
namespace cppuml {
class Class;
}

namespace cppuml {

/**
 * @brief Distingue entre class, struct y union.
 *
 * Todos son modelados por la clase 'Class', pero el exportador
 * (p.ej., PlantUML) necesita saber la diferencia.
 */
enum class ClassKind {
    Class,
    Struct,
    Union
};

/**
 * @brief Modela una 'class', 'struct' o 'union' de C++.
 *
 * Es un Element (tiene nombre/visibilidad) y actúa como un contenedor
 * que posee sus Fields y Methods 
 *
 * También almacena punteros no propietarios a sus clases base
 */
class Class : public Element {
public:
    /**
     * @brief Información sobre una clase base.
     * Utiliza un puntero no propietario 
     */
    struct InheritanceInfo {
        Class* baseClass = nullptr;
        Visibility visibility = Visibility::None;
    };

    /**
     * @brief Constructor para una Clase.
     * @param name El nombre de la clase (p.ej., "MyClass").
     * @param kind El tipo (class, struct, o union).
     */
    Class(std::string name, ClassKind kind = ClassKind::Class)
        : Element(std::move(name)), m_kind(kind) {}

    /**
     * @brief Destructor virtual por defecto.
     */
    ~Class() override = default;

    // --- Implementación de la Interfaz Element ---

    /**
     * @brief Obtiene el tipo de este elemento (para RTTI interno).
     * @return ElementKind::Class.
     */
    ElementKind getKind() const override { return ElementKind::Class; } 

    // --- Acceso a Propiedades de Clase ---

    /**
     * @brief Establece el tipo de definición (class, struct, union).
     */
    void setClassKind(ClassKind kind) { m_kind = kind; } 

    /**
     * @brief Obtiene el tipo de definición (class, struct, union).
     */
    ClassKind getClassKind() const { return m_kind; } 

    // --- Gestión de Miembros

    /**
     * @brief Añade un campo (miembro de datos) a esta clase.
     * La clase toma posesión del campo.
     */
    void addField(std::unique_ptr<Field> field) {
        m_fields.push_back(std::move(field));
    }

    /**
     * @brief Obtiene una vista de solo lectura de los campos.
     */
    const std::vector<std::unique_ptr<Field>>& getFields() const {
        return m_fields;
    }

    /**
     * @brief Añade un método (función miembro) a esta clase.
     * La clase toma posesión del método.
     */
    void addMethod(std::unique_ptr<Method> method) {
        m_methods.push_back(std::move(method));
    }

    /**
     * @brief Obtiene una vista de solo lectura de los métodos.
     */
    const std::vector<std::unique_ptr<Method>>& getMethods() const {
        return m_methods;
    }

    // --- Gestión de Relaciones (Lineamiento P3: Punteros no propietarios) ---

    /**
     * @brief Registra una clase base de la cual esta clase hereda.
     * @param base Puntero no propietario a la definición de la clase base.
     * @param visibility La visibilidad de la herencia (public, protected, private).
     */
    void addBaseClass(Class* base, Visibility visibility) {
        m_baseClasses.push_back(InheritanceInfo{base, visibility});
    }

    /**
     * @brief Obtiene una vista de solo lectura de las relaciones de herencia.
     */
    const std::vector<InheritanceInfo>& getBaseClasses() const {
        return m_baseClasses;
    }

private:
    ClassKind m_kind;
    std::string m_templateParameters; // Para soporte futuro de plantillas

    std::vector<std::unique_ptr<Field>> m_fields;
    std::vector<std::unique_ptr<Method>> m_methods;
    std::vector<InheritanceInfo> m_baseClasses;
};

} // namespace cppuml

#endif // CPP_UML_GENERATOR_CORE_MODEL_CLASS_H