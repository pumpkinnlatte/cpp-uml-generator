#ifndef CPP_UML_GENERATOR_CORE_MODEL_TRANSLATION_UNIT_H
#define CPP_UML_GENERATOR_CORE_MODEL_TRANSLATION_UNIT_H

#include <string>
#include <memory>  // Para std::unique_ptr
#include <utility> // Para std::move

#include "Element.h"
#include "Namespace.h"

namespace cppuml {

/**
 * @brief Modela una Unidad de Traducción de C++ (un solo archivo .cpp o .h).
 *
 * Es el objeto raíz creado por el Parser para un archivo.
 * Su "nombre" (de Element) es la ruta del archivo.
 *
 * Posee el 'namespace global' (::) de ese archivo, que a su vez
 * contiene todos los elementos de nivel superior.
 */
class TranslationUnit : public Element {
public:
    /**
     * @brief Constructor para una Unidad de Traducción.
     * @param filepath La ruta completa al archivo, que se usará como nombre.
     */
    explicit TranslationUnit(std::string filepath)
        : Element(std::move(filepath)) {
        
        // Cada TU tiene un namespace global (::) que posee todo.
        // Lo creamos aquí para asegurar que nunca sea nulo.
        m_globalNamespace = std::make_unique<Namespace>("::");
        
        // La visibilidad no aplica a una TU.
        setVisibility(Visibility::None);
    }

    /**
     * @brief Destructor virtual por defecto.
     */
    ~TranslationUnit() override = default;

    // --- Implementación de la Interfaz Element ---

    /**
     * @brief Obtiene el tipo de este elemento.
     * @return ElementKind::TranslationUnit.
     */
    ElementKind getKind() const override { return ElementKind::TranslationUnit; }

    // --- Acceso Público ---

    /**
     * @brief Obtiene el namespace global (::) de esta unidad de traducción.
     *
     * El Parser usará esto para añadir clases, funciones y namespaces
     * de nivel superior.
     *
     * @return Un puntero no propietario al Namespace global.
     */
    Namespace* getGlobalNamespace() const {
        return m_globalNamespace.get();
    }

private:
    std::unique_ptr<Namespace> m_globalNamespace;
};

} // namespace cppuml

#endif // CPP_UML_GENERATOR_CORE_MODEL_TRANSLATION_UNIT_H