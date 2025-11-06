#ifndef CPP_UML_GENERATOR_CORE_MODEL_NAMESPACE_H
#define CPP_UML_GENERATOR_CORE_MODEL_NAMESPACE_H

#include <string>
#include <vector>
#include <memory>  // Para std::unique_ptr
#include <utility> // Para std::move

#include "Element.h"

namespace cppuml {

/**º
 * @brief Modela un 'namespace' de C++.
 *
 * Actúa como un contenedor genérico para otros elementos (Clases,
 * Namespaces anidados, funciones globales, etc.)
 */
class Namespace : public Element {
public:
    /**
     * @brief Constructor para un Namespace.
     * @param name El nombre del namespace (p.ej., "std", "::" para el global).
     */
    explicit Namespace(std::string name)
        : Element(std::move(name)) {
        // Los Namespaces generalmente no tienen visibilidad (son contenedores)
        setVisibility(Visibility::None);
    }

    /**
     * @brief Destructor virtual por defecto.
     */
    ~Namespace() override = default;

    // --- Implementación de la Interfaz Element ---

    /**
     * @brief Obtiene el tipo de este elemento.
     * @return ElementKind::Namespace.
     */
    ElementKind getKind() const override { return ElementKind::Namespace; }

    // --- Gestión de Miembros ---

    /**
     * @brief Añade un elemento hijo (Class, Method, Field, Namespace) a este namespace.
     * El namespace toma posesión del elemento.
     *
     * @param member Un unique_ptr al Elemento hijo.
     */
    void addMember(std::unique_ptr<Element> member) {
        m_members.push_back(std::move(member));
    }

    /**
     * @brief Obtiene una vista de solo lectura de los elementos hijos.
     *
     * Esto permite a los visitantes (p.ej., exportadores) iterar sobre
     * clases, namespaces anidados, funciones globales, etc.
     */
    const std::vector<std::unique_ptr<Element>>& getMembers() const {
        return m_members;
    }

private:
    std::vector<std::unique_ptr<Element>> m_members;
};

} // namespace cppuml

#endif // CPP_UML_GENERATOR_CORE_MODEL_NAMESPACE_H