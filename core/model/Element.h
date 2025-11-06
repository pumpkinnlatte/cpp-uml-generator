#ifndef CPP_UML_GENERATOR_CORE_MODEL_ELEMENT_H
#define CPP_UML_GENERATOR_CORE_MODEL_ELEMENT_H

#include <string>
#include <utility> // Para std::move

namespace cppuml {

/**
 * @brief Define la visibilidad de un elemento del modelo (p.ej., public, private).
 */
enum class Visibility {
    None,      ///< Visibilidad no aplicable (p.ej., Namespace global)
    Public,
    Protected,
    Private
};

/**
 * @brief Define el tipo de un elemento del modelo (para "RTTI" interno).
 *
 * Esto nos permite realizar conversiones seguras (p.ej., de Element* a Class*)
 * sin depender de dynamic_cast.
 */
enum class ElementKind {
    Element,
    Type,
    Field,
    Method,
    Class,
    Namespace,
    TranslationUnit,
    Model
};

/**
 * @brief Clase base abstracta para todos los elementos del modelo (Clase, Método, Campo, etc.).
 *
 * Esta clase establece la interfaz común (nombre, visibilidad) y asegura
 * el polimorfismo correcto a través de un destructor virtual.
 *
 * Diseñado para ser propiedad de un std::unique_ptr, por lo tanto, no es copiable.
 */
class Element {
public:
    /**
     * @brief Constructor que inicializa el elemento con un nombre.
     * @param name El nombre del elemento (p.ej., "MyClass", "m_member").
     */
    explicit Element(std::string name)
        : m_name(std::move(name)), m_visibility(Visibility::None) {}

    /**
     * @brief Destructor virtual por defecto.
     * Crítico para la eliminación polimórfica (Lineamiento P2).
     */
    virtual ~Element() = default;

    // --- Semántica de Movimiento y Copia ---
    
    /**
     * Los elementos no son copiables. Tienen una identidad única y son
     * propiedad de un std::unique_ptr 
     */
    Element(const Element&) = delete;
    Element& operator=(const Element&) = delete;

    /**
     * Los elementos son movibles (requerido por std::unique_ptr).
     */
    Element(Element&&) = default;
    Element& operator=(Element&&) = default;

    // --- Acceso Público ---

    /**
     * @brief Obtiene el tipo de este elemento (p.ej., Class, Method).
     * @return El ElementKind de la clase derivada.
     */
    virtual ElementKind getKind() const = 0;

    /**
     * @brief Obtiene el nombre del elemento.
     */
    const std::string& getName() const { return m_name; }

    /**
     * @brief Establece el nombre del elemento.
     */
    void setName(std::string name) { m_name = std::move(name); }

    /**
     * @brief Obtiene la visibilidad (public, protected, private) del elemento.
     */
    Visibility getVisibility() const { return m_visibility; }

    /**
     * @brief Establece la visibilidad del elemento.
     */
    void setVisibility(Visibility visibility) { m_visibility = visibility; }

private:
    std::string m_name;
    Visibility m_visibility;
};

} // namespace cppuml

#endif // CPP_UML_GENERATOR_CORE_MODEL_ELEMENT_H